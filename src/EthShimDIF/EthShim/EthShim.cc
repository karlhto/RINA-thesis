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

EthShim::EthShim() : resolving(false)
{
}

// TODO delete messages in queue
EthShim::~EthShim()
{
}

void EthShim::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        ipcProcess = getParentModule();
        arp = getRINAModule<RINArp *>(this, 1, {"arp"});
        shimFA = getRINAModule<ShimFA *>(this, 1, {MOD_FLOWALLOC, MOD_FA});

        arp->subscribe(RINArp::completedRINArpResolutionSignal, this);
        arp->subscribe(RINArp::failedRINArpResolutionSignal, this);
    } else if (stage == inet::INITSTAGE_NETWORK_LAYER) {
        // Get correct interface entry
        auto ift = inet::getModuleFromPar<inet::IInterfaceTable>(par("interfaceTableModule"), this);
        // FIXME please use something that doesn't suck
        ie = ift->getInterface(0);
        if (ie == nullptr)
            throw cRuntimeError("Interface entry is required for shim module to work");
    }
}

void EthShim::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("arpIn")) {
        EV_INFO << "Received Arp packet." << endl;
        sendPacketToNIC(PK(msg));
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
        SDUData *sdu = check_and_cast<SDUData *>(msg);
        handleSDU(sdu, gate);
    }
}

bool EthShim::addPort(const APN &dstApn, const int &portId)
{
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

    gateMap[shimIn] = dstApn;
    return true;
}

void EthShim::handleSDU(SDUData *sdu, cGate *gate)
{
    EV_INFO << "Doing stuff" << endl;
    const APN &dstApn = gateMap[gate];
    inet::MACAddress mac = arp->resolveAddress(dstApn);

    if (mac.isUnspecified()) {
        insertSDU(sdu, dstApn, outQueue);
        resolving = true;
        return;
    }

    // TODO need DIF name here as VLAN ID, too
    // TODO additionally need hashing function for DIF name so it fits into
    //      12 bits (which is size of VLAN tag)

    inet::Ieee802Ctrl *controlInfo = new inet::Ieee802Ctrl();
    controlInfo->setDest(mac);
    controlInfo->setEtherType(inet::ETHERTYPE_INET_GENERIC);
    sdu->setControlInfo(controlInfo);
    sendPacketToNIC(sdu);
}

void EthShim::handleIncomingSDU(SDUData *sdu)
{
    EV_INFO << "Passing SDU to correct gate" << endl;

    inet::Ieee802Ctrl *ctrlInfo = check_and_cast<inet::Ieee802Ctrl *>(sdu->getControlInfo());
    const inet::MACAddress &srcMac = ctrlInfo->getSourceAddress();
    const APN srcApn = arp->getAddressFor(srcMac);
    if (srcApn.isUnspecified()) {
        EV_WARN << "ARP Resolved wrong address, " << endl;
        delete sdu;  // just place in a queue with discard timer?
        return;
    }

    EV_INFO << "SDU was from destination application " << srcApn << endl;

    cGate *gate = nullptr;
    for (const auto &elem : gateMap)
        if (elem.second == srcApn)
            gate = elem.first;

    if (gate == nullptr) {
        insertSDU(sdu, srcApn, inQueue);
        shimFA->createUpperFlow(srcApn);
        return;
    }

    // send to output gate
    send(sdu, gate->getOtherHalf());
}

void EthShim::sendWaitingSDUs(const APN &srcApn)
{
    cGate *gate = nullptr;
    for (const auto &elem : gateMap)
        if (elem.second == srcApn)
            gate = elem.first;

    if (gate == nullptr) {
        EV_ERROR << "Called to send SDUs from " << srcApn << ", but no gate present" << endl;
        return;
    }

    auto &vec = inQueue[srcApn];
    for (SDUData *sdu : vec)
        send(sdu, gate->getOtherHalf());
}

void EthShim::insertSDU(SDUData *sdu, const APN &apn, queueMap &queue)
{
    auto &vec = queue[apn];
    vec.push_back(sdu);
}

void EthShim::handleIncomingArpPacket(RINArpPacket *arpPacket)
{
    EV_INFO << "Sending " << arpPacket << " to arp." << endl;
    send(arpPacket, "arpOut");
}

void EthShim::sendPacketToNIC(cPacket *packet)
{
    EV_INFO << "Sending " << packet << " to ethernet interface." << endl;
    send(packet, "ifOut");
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
    if (!resolving)
        return;

    if (signalID == RINArp::completedRINArpResolutionSignal)
        arpResolutionCompleted(check_and_cast<RINArp::ArpNotification *>(obj));
    else if (signalID == RINArp::failedRINArpResolutionSignal)
        arpResolutionFailed(check_and_cast<RINArp::ArpNotification *>(obj));
    else
        throw cRuntimeError("Unsubscribed signalID triggered receiveSignal");
}

void EthShim::arpResolutionCompleted(RINArp::ArpNotification *entry)
{
    Enter_Method("arpResolutionCompleted(%s -> %s)", entry->apName.getName().c_str(),
                 entry->macAddress.str().c_str());

    auto &vec = outQueue[entry->apName];
    for (SDUData *sdu : vec) {
        inet::Ieee802Ctrl *controlInfo = new inet::Ieee802Ctrl();
        controlInfo->setDest(entry->macAddress);
        controlInfo->setEtherType(inet::ETHERTYPE_INET_GENERIC);
        sdu->setControlInfo(controlInfo);
        sendPacketToNIC(sdu);
    }
    resolving = false;
}

void EthShim::arpResolutionFailed(RINArp::ArpNotification *entry)
{
    (void)entry;
    // discard all sdu entries for given APN, or retain?
}
