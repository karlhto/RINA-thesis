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

#include "Common/SDUData_m.h"
#include "Common/Utils.h"
#include "EthShimDIF/RINArp/RINArpPacket_m.h"
#include "EthShimDIF/ShimFA/ShimFA.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/common/Ieee802Ctrl.h"
#include "inet/networklayer/contract/IInterfaceTable.h"


Define_Module(EthShim);

EthShim::EthShim() : flows() {}

// TODO (karlhto): delete messages in queue
EthShim::~EthShim() = default;

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
            unsigned int vlanID = std::stoul(difName, &rest, 10);
            if (rest < difName.length()) {
                throw std::invalid_argument("");
            }

            // TODO (karlhto): use some constant for this (hopefully already supplied)
            if (vlanID >= 4096) {
            }
        } catch (std::invalid_argument) {
            throw cRuntimeError("DIF name not valid: %s", difName.c_str());
        }
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
        sendPacketToNIC(msg);
    } else if (msg->arrivedOn("ifIn")) {
        EV_INFO << "Received " << msg << " from network." << endl;
        if (auto arpPacket = dynamic_cast<RINArpPacket *>(msg))
            handleIncomingArpPacket(arpPacket);
        else if (auto sdu = dynamic_cast<SDUData *>(msg))
            handleIncomingSDU(sdu);
        else
            throw cRuntimeError(msg, "Unsupported message type");
    } else {
        cGate *gate = msg->getArrivalGate();
        EV_INFO << "Received PDU from upper layer." << endl;
        auto *sdu = check_and_cast<SDUData *>(msg);
        handleSDU(sdu, gate);
    }
}

bool EthShim::addPort(const APN &dstApn, const int &portId)
{
    auto &entry = flows[dstApn];
    ASSERT(entry != nullptr);

    std::ostringstream gateName;
    gateName << GATE_NORTHIO_ << portId;
    const std::string &tmp = gateName.str();
    const char *gateStr = tmp.c_str();

    addGate(gateStr, cGate::INOUT, false);
    cGate *shimIn = gateHalf(gateStr, cGate::INPUT);
    cGate *shimOut = gateHalf(gateStr, cGate::OUTPUT);

    if (!ipcProcess->hasGate(gateStr))
        ipcProcess->addGate(gateStr, cGate::INOUT, false);
    cGate *ipcDownIn = ipcProcess->gateHalf(gateStr, cGate::INPUT);
    cGate *ipcDownOut = ipcProcess->gateHalf(gateStr, cGate::OUTPUT);

    shimOut->connectTo(ipcDownOut);
    ipcDownIn->connectTo(shimIn);
    if (!shimOut->isConnected() || !shimIn->isConnected())
        return false;

    entry->gate = shimIn;
    if (entry->state == PENDING)
        entry->state = ALLOCATED;

    return true;
}


// TODO (karlhto): Add ENUM for more granularity?
bool EthShim::createEntry(const APN &dstApn)
{
    auto &entry = flows[dstApn];
    if (entry != nullptr) {
        EV_ERROR << "ShimEntry already exists, something has probably gone wrong" << endl;
        return false;
    }

    entry = std::make_unique<ShimEntry>();
    entry->state = PENDING;

    auto &mac = arp->resolveAddress(dstApn);
    if (!mac.isUnspecified())
        return true;

    // This is not very idiomatic: false here is still a good thing
    return false;
}

void EthShim::handleSDU(SDUData *sdu, cGate *gate)
{
    EV_INFO << "Doing stuff" << endl;

    // TODO (karlhto): split into separate function so we can use references instead
    const APN *dstApn = nullptr;
    for (auto &shimEntry : flows) {
        if (shimEntry.second->gate == gate) {
            dstApn = &shimEntry.first;
        }
    }

    if (dstApn == nullptr) {
        EV_ERROR << "Gate " << gate->getName() << " does not belong to any shim entry" << endl;
        return;
    }

    inet::MACAddress mac = arp->resolveAddress(*dstApn);

    if (mac.isUnspecified()) {
        insertOutgoingSDU(*dstApn, sdu);
        return;
    }

    auto *controlInfo = new inet::Ieee802Ctrl();
    controlInfo->setDest(mac);
    controlInfo->setEtherType(inet::ETHERTYPE_INET_GENERIC);
    sdu->setControlInfo(controlInfo);
    sendPacketToNIC(sdu);
}

void EthShim::handleIncomingSDU(SDUData *sdu)
{
    EV_INFO << "Passing SDU to correct gate" << endl;

    auto *ctrlInfo = check_and_cast<inet::Ieee802Ctrl *>(sdu->getControlInfo());
    const inet::MACAddress &srcMac = ctrlInfo->getSourceAddress();
    const APN &srcApn = arp->getAddressFor(srcMac);
    if (srcApn.isUnspecified()) {
        EV_WARN << "ARP does not have a valid entry for source MAC " << srcApn << endl;
        // TODO (karlhto): Consider whether having some queue for this is better
        delete sdu;
        return;
    }

    auto &entry = flows[srcApn];
    if (entry == nullptr) {
        entry = std::make_unique<ShimEntry>();
        entry->state = PENDING;
        insertIncomingSDU(srcApn, sdu);
        shimFA->createUpperFlow(srcApn);
        return;
    }

    if (entry->state == PENDING) {
        insertIncomingSDU(srcApn, sdu);
        return;
    }

    auto &gate = entry->gate;
    ASSERT(gate != nullptr);

    // send to output gate
    send(sdu, gate->getOtherHalf());
}

void EthShim::sendWaitingSDUs(const APN &srcApn)
{
    auto &entry = flows[srcApn];
    ASSERT(entry != nullptr);

    cGate *gate = entry->gate;
    // TODO (karlhto): Possibly better with assert here
    if (gate == nullptr) {
        EV_ERROR << "Called to send SDUs from " << srcApn << ", but no gate present" << endl;
        return;
    }

    auto &queue = entry->inQueue;
    while (!queue.empty()) {
        auto &sdu = queue.front();
        send(sdu, gate->getOtherHalf());
        queue.pop();
    }
}

void EthShim::insertOutgoingSDU(const APN &apn, SDUData *sdu)
{
    auto &entry = flows[apn];
    ASSERT(entry != nullptr);
    entry->outQueue.push(sdu);
}

void EthShim::insertIncomingSDU(const APN &apn, SDUData *sdu)
{
    auto &entry = flows[apn];
    ASSERT(entry != nullptr);
    entry->inQueue.push(sdu);
}

void EthShim::handleIncomingArpPacket(RINArpPacket *arpPacket)
{
    EV_INFO << "Sending " << arpPacket << " to arp." << endl;
    send(arpPacket, "arpOut");
}

void EthShim::sendPacketToNIC(cMessage *msg)
{
    EV_INFO << "Sending " << msg << " to ethernet interface." << endl;
    send(msg, "ifOut");
}

void EthShim::registerApplication(const APN &apni) const
{
    Enter_Method("registerApplication(%s)", apni.getName().c_str());
    EV_INFO << "Received request to register application name " << apni << " with Arp module."
            << endl;

    inet::MACAddress mac = ie->getMacAddress();
    arp->addStaticEntry(mac, apni);
}

void EthShim::receiveSignal(cComponent *, simsignal_t signalID, cObject *obj, cObject *)
{
    if (signalID == RINArp::completedRINArpResolutionSignal)
        arpResolutionCompleted(check_and_cast<RINArp::ArpNotification *>(obj));
    else if (signalID == RINArp::failedRINArpResolutionSignal)
        arpResolutionFailed(check_and_cast<RINArp::ArpNotification *>(obj));
    else
        throw cRuntimeError("Unsubscribed signalID triggered receiveSignal");
}

void EthShim::arpResolutionCompleted(RINArp::ArpNotification *notification)
{
    const APN &apn = notification->getApName();
    const inet::MACAddress &mac = notification->getMacAddress();
    auto &entry = flows[apn];
    if (entry == nullptr) {
        // Do nothing
        return;
    }

    Enter_Method("arpResolutionCompleted(%s -> %s)", apn.getName().c_str(), mac.str().c_str());

    if (entry->state == PENDING) {
        shimFA->completedAddressResolution(apn);
        return;
    }

    // TODO (karlhto): Consider more states
    auto &queue = entry->outQueue;
    while (!queue.empty()) {
        auto &sdu = queue.front();
        auto *controlInfo = new inet::Ieee802Ctrl();
        controlInfo->setDest(mac);
        // TODO (karlhto): Check possibility for using 0xD1F0 as ethertype
        controlInfo->setEtherType(inet::ETHERTYPE_INET_GENERIC);
        sdu->setControlInfo(controlInfo);
        sendPacketToNIC(sdu);
        queue.pop();
    }
}

void EthShim::arpResolutionFailed(RINArp::ArpNotification *entry)
{
    (void)entry;
    // discard all SDU entries, deallocate flow?
}
