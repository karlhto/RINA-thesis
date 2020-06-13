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
#include "EthShimDIF/RINArp/RINArpPacket_m.h"
#include "EthShimDIF/ShimFA/ShimFA.h"
#include "inet/linklayer/common/Ieee802Ctrl.h"


Define_Module(EthShim);

EthShim::EthShim() {}

EthShim::~EthShim() {}

void EthShim::initialize()
{
    initPointers();
    initGates();
}

void EthShim::initPointers()
{
    ipcProcess = getModuleByPath("^");
    arp = dynamic_cast<RINArp *>(ipcProcess->getSubmodule("arp"));
    if (arp == nullptr)
        throw cRuntimeError("EthShim needs ARP module");
    shimFA = dynamic_cast<ShimFA *>(ipcProcess->getModuleByPath(".flowAllocator.fa"));
    if (shimFA == nullptr)
        throw cRuntimeError("EthShim needs ShimFlowAllocator module");
}

void EthShim::initGates()
{
    arpIn = gate("arpIn");
    arpOut = gate("arpOut");
    ifIn = gate("ifIn");
    ifOut = gate("ifOut");
}

void EthShim::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn(arpIn->getId())) {
        EV_INFO << "Received ARP packet." << endl;
        sendPacketToNIC(PK(msg));
    } else if (msg->arrivedOn(ifIn->getId())) {
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

bool EthShim::addPort(const APN &dstApn, int portId) {
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

void EthShim::handleSDU(SDUData *sdu, cGate *gate) {
    EV_INFO << "Doing stuff" << endl;
    const APN &dstAddr = gateMap[gate];
    inet::MACAddress mac = arp->resolveAddress(dstAddr);

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
    (void)sdu;
    EV_INFO << "Should be passed to connected IPCP" << endl;
    // TODO ask ARP if we know this one to find dstApn

    inet::Ieee802Ctrl *ctrlInfo = check_and_cast<inet::Ieee802Ctrl *>(sdu->getControlInfo());
    const inet::MACAddress &srcMac = ctrlInfo->getSourceAddress();
    const APN srcApn = arp->getAddressFor(srcMac);
    if (srcApn.isUnspecified()) {
        delete sdu; // just place in a queue with discard timer?
        return;
    }

    cGate *gate = nullptr;
    for (const auto &elem : gateMap)
        if (elem.second == srcApn)
            gate = elem.first;

    if (gate == nullptr) {
        insertSDU(sdu, srcApn);
        shimFA->createUpperFlow(srcApn);
        return;
    }
    // TODO if no dstApn, ask FA to pass creation request to upper layer,
    //      store this in a queue of some kind of queue that becomes

    // TODO if dstApn, forward it to correct gate and we're good to go
}

void EthShim::insertSDU(SDUData *sdu, const APN &srcApn)
{
    auto vec = &(queue[srcApn]);
    vec->push_back(sdu);
}

void EthShim::handleIncomingArpPacket(RINArpPacket *arpPacket)
{
    EV_INFO << "Sending " << arpPacket << " to arp." << endl;
    send(arpPacket, arpOut);
}

void EthShim::sendPacketToNIC(cPacket *packet)
{
    EV_INFO << "Sending " << packet << " to ethernet interface." << endl;
    send(packet, ifOut);
}

void EthShim::registerApplication(const APN &apni) const
{
    Enter_Method("registerApplication(%s)", apni.getName().c_str());
    EV_INFO << "Received request to register application name " << apni << " with ARP module."
            << endl;

    inet::MACAddress mac = getMacAddressOfNIC();
    arp->addStaticEntry(mac, apni);
}

const inet::MACAddress EthShim::getMacAddressOfNIC() const
{
    cModule *mac = ipcProcess->getModuleByPath(".eth.mac");
    if (mac == nullptr)
        throw cRuntimeError("Unable to get address from MAC interface");
    if (!mac->hasPar("address"))
        throw cRuntimeError("MAC interface has no address parameter");
    return inet::MACAddress(mac->par("address").stringValue());
}

void EthShim::receiveSignal(cComponent *, simsignal_t signalID, cObject *obj, cObject *)
{
    Enter_Method_Silent();

    if (signalID == RINArp::completedRINArpResolutionSignal)
        arpResolutionCompleted(check_and_cast<RINArp::ArpNotification *>(obj));
    else if (signalID == RINArp::failedRINArpResolutionSignal)
        arpResolutionFailed(check_and_cast<RINArp::ArpNotification *>(obj));
    else
        throw cRuntimeError("Unsubscribed signalID triggered receiveSignal");
}

void EthShim::arpResolutionCompleted(RINArp::ArpNotification *entry)
{
    (void)entry;
    // something about notifying SDU queues here
}

void EthShim::arpResolutionFailed(RINArp::ArpNotification *entry)
{
    (void)entry;
    // something about notifying SDU queues here
}

/* How to handle delimiting? Should the delimiting module be reused, should the
 * upper layer be forced to deliver packets that are 1500 bytes long, or
 * should there be an internal delimiting module?
 */
