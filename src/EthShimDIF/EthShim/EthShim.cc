// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Brno University of Technology, PRISTINE project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "EthShimDIF/EthShim/EthShim.h"
#include <algorithm>

#include "Common/SDUData_m.h"
#include "Common/Utils.h"
#include "EthShimDIF/RINArp/RINArpPacket_m.h"
#include "EthShimDIF/ShimFA/ShimFA.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/common/Ieee802Ctrl.h"
#include "inet/networklayer/contract/IInterfaceTable.h"

// ETHERTYPE "extension" for RINA
#define ETHERTYPE_RINA 0xD1F0

Define_Module(EthShim);

void EthShim::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        ipcProcess = getParentModule();
        arp = getRINAModule<RINArp *>(this, 1, {"arp"});
        shimFA = getRINAModule<ShimFA *>(this, 1, {MOD_FLOWALLOC, MOD_FA});

        arp->subscribe(RINArp::completedRINArpResolutionSignal, this);
        arp->subscribe(RINArp::failedRINArpResolutionSignal, this);

        // FIXME INET 3.6.7 does not support VLANs, so this is intended for use when support for
        //       INET >4.1 is implemented. This will probably require proper serialisation of
        //       everything though
        std::string difName = ipcProcess->par("difName").stringValue();
        try {
            std::string::size_type rest;
            unsigned int tmpId = std::stoul(difName, &rest, 10);
            if (rest < difName.length())
                throw std::invalid_argument("");

            // TODO (karlhto): do this in a cleaner way, maybe INET has some method for it
            // VLAN IDs 0 and 4095 (where 4095 is max, 2^12) are reserved
            if (tmpId < 1 || tmpId >= 4095)
                throw std::invalid_argument("");

            vlanId = tmpId;
        } catch (std::invalid_argument) {
            throw cRuntimeError("DIF name for shim IPCP must be a valid VLAN ID, not: %s",
                                difName.c_str());
        }

        WATCH(numSentToNetwork);
        WATCH(numReceivedFromNetwork);
        WATCH(vlanId);
        WATCH_MAP(connections);
    } else if (stage == inet::INITSTAGE_NETWORK_LAYER) {
        // Get correct interface entry
        auto ift = inet::getModuleFromPar<inet::IInterfaceTable>(par("interfaceTableModule"), this);
        cModule *eth = ipcProcess->getModuleByPath(".eth");
        ie = ift->getInterfaceByInterfaceModule(eth);
        if (ie == nullptr)
            throw cRuntimeError("Interface entry is required for shim module to work");
    }
}

void EthShim::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("arpIn")) {
        EV_INFO << "Received Arp packet." << endl;
        sendPacketToNIC(check_and_cast<cPacket *>(msg));
    } else if (msg->arrivedOn("ifIn")) {
        EV_INFO << "Received " << msg << " from network." << endl;
        numReceivedFromNetwork++;
        if (auto arpPacket = dynamic_cast<RINArpPacket *>(msg))
            handleIncomingArpPacket(arpPacket);
        else if (auto sdu = dynamic_cast<SDUData *>(msg))
            handleIncomingSDU(sdu);
        else
            throw cRuntimeError(msg, "Unsupported message type");
    } else {
        EV_INFO << "Received PDU from upper layer" << endl;
        auto *sdu = check_and_cast<SDUData *>(msg);
        handleOutgoingSDU(sdu, sdu->getArrivalGate());
    }
}

void EthShim::handleOutgoingSDU(SDUData *sdu, const cGate *gate)
{
    EV_INFO << "Sending packet over ethernet" << endl;
    ASSERT(gate != nullptr);

    auto iter = std::find_if(connections.begin(), connections.end(),
                             [gate](const auto &iter) { return iter.second.inGate == gate; });
    ASSERT(iter != connections.end());

    const APN &dstApn = iter->first;
    const inet::MACAddress &mac = arp->resolveAddress(dstApn);
    if (mac.isUnspecified()) {
        // This means that resolution has started
        ConnectionEntry &entry = iter->second;
        ASSERT(entry.state != ConnectionState::none);
        entry.outQueue.insert(sdu);
        return;
    }

    auto *controlInfo = new inet::Ieee802Ctrl();
    controlInfo->setDest(mac);
    controlInfo->setEtherType(ETHERTYPE_RINA);
    sdu->setControlInfo(controlInfo);
    sendPacketToNIC(sdu);
}

void EthShim::handleIncomingSDU(SDUData *sdu)
{
    EV_INFO << "Passing SDU to correct gate" << endl;

    auto *ctrlInfo = check_and_cast<inet::Ieee802Ctrl *>(sdu->removeControlInfo());
    const inet::MACAddress &srcMac = ctrlInfo->getSourceAddress();
    const APN &srcApn = arp->getAddressFor(srcMac);
    delete ctrlInfo;
    if (srcApn.isUnspecified()) {
        EV_WARN << "ARP does not have a valid entry for source MAC " << srcMac
                << ". Dropping packet." << endl;
        delete sdu;
        return;
    }

    EV_INFO << "SDU is from application with name " << srcApn << endl;

    auto &entry = connections[srcApn];
    if (entry.state == ConnectionState::none) {
        EV_INFO << "No connection entry with destination application name " << srcApn
                << ". Creating a new one and passing request to Flow Allocator." << endl;
        entry.state = ConnectionState::pending;
        entry.inQueue.insert(sdu);
        shimFA->createUpperFlow(srcApn);
        return;
    }

    if (entry.state == ConnectionState::pending) {
        EV_INFO << "Connection with application " << srcApn
                << " is still pending, SDU inserted into queue." << endl;
        entry.inQueue.insert(sdu);
        return;
    }

    EV_INFO << "Sending SDU to upper layer" << endl;
    send(sdu, entry.outGate);
}

void EthShim::handleIncomingArpPacket(RINArpPacket *arpPacket)
{
    EV_INFO << "Sending " << arpPacket << " to ARP module." << endl;
    send(arpPacket, "arpOut");
}

void EthShim::sendPacketToNIC(cPacket *packet)
{
    EV_INFO << "Sending " << packet << " to ethernet interface." << endl;
    auto *controlInfo = dynamic_cast<inet::Ieee802Ctrl *>(packet->getControlInfo());
    ASSERT(controlInfo != nullptr);
    controlInfo->setInterfaceId(ie->getInterfaceId());
    numSentToNetwork++;
    send(packet, "ifOut");
}

void EthShim::sendWaitingIncomingSDUs(const APN &srcApn)
{
    auto &entry = connections[srcApn];
    ASSERT(entry.state == ConnectionState::allocated);
    ASSERT(entry.outGate != nullptr);

    auto &queue = entry.inQueue;
    while (!queue.isEmpty()) {
        cPacket *sdu = queue.pop();
        send(sdu, entry.outGate);
    }
}

bool EthShim::createBindingsForEntry(ConnectionEntry &entry, const int portId)
{
    ASSERT(portId >= 0 && portId < ShimFA::MAX_PORTID);
    ASSERT(entry.state == ConnectionState::pending);

    std::ostringstream gateName;
    gateName << GATE_NORTHIO_ << portId;
    const std::string &tmp = gateName.str();
    const char *gateStr = tmp.c_str();

    ASSERT(!hasGate(gateStr));

    addGate(gateStr, cGate::INOUT, false);
    cGate *shimIn = gateHalf(gateStr, cGate::INPUT);
    cGate *shimOut = gateHalf(gateStr, cGate::OUTPUT);

    // This gate should already be created
    if (!ipcProcess->hasGate(gateStr))
        ipcProcess->addGate(gateStr, cGate::INOUT, false);
    cGate *ipcDownIn = ipcProcess->gateHalf(gateStr, cGate::INPUT);
    cGate *ipcDownOut = ipcProcess->gateHalf(gateStr, cGate::OUTPUT);

    shimOut->connectTo(ipcDownOut);
    ipcDownIn->connectTo(shimIn);
    if (!shimOut->isConnected() || !shimIn->isConnected())
        return false;

    entry.inGate = shimIn;
    entry.outGate = shimOut;

    return true;
}

void EthShim::removeBindingsForEntry(ConnectionEntry &entry)
{
    if (entry.inGate == nullptr)
        return;

    const char *basename = entry.inGate->getBaseName();
    if (ipcProcess->hasGate(basename)) {
        cGate *ipcDownIn = ipcProcess->gateHalf(basename, cGate::INPUT);
        ipcDownIn->disconnect();
        entry.outGate->disconnect();
    }

    deleteGate(basename);
    entry.inGate = nullptr;
    entry.outGate = nullptr;
}

bool EthShim::finalizeConnection(const APN &dstApn, const int portId)
{
    Enter_Method("finalizeConnection(%s, %d)", dstApn.c_str(), portId);
    auto &entry = connections[dstApn];
    if (entry.state != ConnectionState::pending) {
        EV_ERROR << "Connection entry for destination application with name " << dstApn
                 << " cannot be finalized since state is not pending. Current state is: "
                 << entry.state << endl;
        return false;
    }

    const bool isConnected = createBindingsForEntry(entry, portId);
    if (!isConnected) {
        EV_ERROR << "Bindings for entry with destination address " << dstApn
                 << " could not be created" << endl;
        deleteEntry(dstApn);
        return false;
    }

    entry.state = ConnectionState::allocated;
    sendWaitingIncomingSDUs(dstApn);
    return true;
}

EthShim::CreateResult EthShim::createEntry(const APN &dstApn)
{
    Enter_Method("createEntry(%s)", dstApn.c_str());
    auto &entry = connections[dstApn];
    if (entry.state != ConnectionState::none) {
        EV_ERROR << "ConnectionEntry already exists, something has probably gone wrong" << endl;
        return CreateResult::error;
    }

    entry.state = ConnectionState::pending;

    EV_INFO << "Initiating ARP resolution for destination address " << dstApn << endl;
    auto &mac = arp->resolveAddress(dstApn);
    if (!mac.isUnspecified()) {
        EV_INFO << "ARP entry was found!" << endl;
        return CreateResult::completed;
    }

    return CreateResult::pending;
}

void EthShim::deleteEntry(const APN &dstApn)
{
    Enter_Method("deleteEntry(%s)", dstApn.c_str());
    auto &entry = connections[dstApn];
    if (entry.state == ConnectionState::none)
        return;

    removeBindingsForEntry(entry);
    // Destructor of cPacketQueue will fix deletion of cPackets
    connections.erase(dstApn);
}

void EthShim::registerApplication(const APN &apni) const
{
    Enter_Method("registerApplication(%s)", apni.c_str());
    EV_INFO << "Received request to register application name " << apni << " with Arp module."
            << endl;

    inet::MACAddress mac = ie->getMacAddress();
    arp->addStaticEntry(mac, apni);
}

void EthShim::receiveSignal(cComponent *src, simsignal_t id, cObject *obj, cObject *detail)
{
    if (id == RINArp::completedRINArpResolutionSignal)
        arpResolutionCompleted(check_and_cast<RINArp::ArpNotification *>(obj));
    else if (id == RINArp::failedRINArpResolutionSignal)
        arpResolutionFailed(check_and_cast<RINArp::ArpNotification *>(obj));
    else
        throw cRuntimeError("Unsubscribed signalID triggered receiveSignal");

    // Unused
    (void)src;
    (void)detail;
}

void EthShim::arpResolutionCompleted(const RINArp::ArpNotification *notification)
{
    const APN &apn = notification->getApName();
    const inet::MACAddress &mac = notification->getMacAddress();
    auto &entry = connections[apn];
    if (entry.state == ConnectionState::none)
        return;

    Enter_Method("arpResolutionCompleted(%s -> %s)", apn.c_str(), mac.str().c_str());

    if (entry.state == ConnectionState::pending) {
        shimFA->completedAddressResolution(apn);
        return;
    }

    auto &queue = entry.outQueue;
    while (!queue.isEmpty()) {
        cPacket *sdu = queue.pop();
        auto *controlInfo = new inet::Ieee802Ctrl();
        controlInfo->setDest(mac);
        controlInfo->setEtherType(ETHERTYPE_RINA);
        sdu->setControlInfo(controlInfo);
        sendPacketToNIC(sdu);
    }
}

void EthShim::arpResolutionFailed(const RINArp::ArpNotification *notification)
{
    const APN &apn = notification->getApName();
    const inet::MACAddress &mac = notification->getMacAddress();
    auto &entry = connections[apn];
    if (entry.state == ConnectionState::none)
        return;

    Enter_Method("arpResolutionFailed(%s -> %s)", apn.c_str(), mac.str().c_str());

    // discard all SDU entries, deallocate flow?
}

std::ostream &operator<<(std::ostream &os, const EthShim::ConnectionState &connectionState)
{
    // No default case so warning will be supplied if someone ever changes ConnectionState
    switch (connectionState) {
    case EthShim::ConnectionState::none:
        os << "NONE";
        break;
    case EthShim::ConnectionState::pending:
        os << "PENDING";
        break;
    case EthShim::ConnectionState::allocated:
        os << "ALLOCATED";
        break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const EthShim::ConnectionEntry &connectionEntry)
{
    os << "State: " << connectionEntry.state;
    os << ", Gate: ";
    if (connectionEntry.inGate != nullptr)
        os << connectionEntry.inGate->getBaseName();
    else
        os << "undefined";
    os << ", In-queue size: " << connectionEntry.inQueue.getLength();
    os << ", Out-queue size: " << connectionEntry.outQueue.getLength();
    return os;
}
