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
#include "EthShimDIF/ShimFA/ShimFA.h"
#include "EthShimDIF/RINArp/RINArpPacket_m.h"
#include "Common/PDU.h"

Define_Module(EthShim);

EthShim::EthShim() {
}

EthShim::~EthShim() {
}

void EthShim::initialize() {
    initPointers();
    initGates();

    // Sets up listening for this module
    arp->subscribe(RINArp::completedRINArpResolutionSignal, this);
    arp->subscribe(RINArp::failedRINArpResolutionSignal, this);
}

void EthShim::initPointers() {
    ipcProcess = getModuleByPath("^");
    arp = dynamic_cast<RINArp *>(ipcProcess->getSubmodule("arp"));
    if (arp == nullptr)
        throw cRuntimeError("EthShim needs ARP module");
    shimFA = dynamic_cast<ShimFA *>(ipcProcess->getModuleByPath(".flowAllocator.fa"));
    if (shimFA == nullptr)
        throw cRuntimeError("EthShim needs ShimFlowAllocator module");
}

void EthShim::initGates() {
    northIn = gate("northIo$i");
    northOut = gate("northIo$o");
    arpIn = gate("arpIn");
    arpOut = gate("arpOut");
    ifIn = gate("ifIn");
    ifOut = gate("ifOut");
}

void EthShim::handleMessage(cMessage *msg) {
    if (msg->arrivedOn(northIn->getId())) {
        EV_INFO << "Received PDU from upper layer." << endl;
        PDU *pdu = check_and_cast<PDU *>(msg);
        handlePDU(pdu);
    } else if (msg->arrivedOn(arpIn->getId())) {
        EV_INFO << "Received ARP packet." << endl;
        sendPacketToNIC(PK(msg));
    } else if (msg->arrivedOn(ifIn->getId())) {
        EV_INFO << "Received " << msg << " from network." << endl;
        if (auto arpPacket = dynamic_cast<RINArpPacket *>(msg))
            handleIncomingArpPacket(arpPacket);
        else if (auto pdu = dynamic_cast<PDU *>(msg))
            handleIncomingPDU(pdu);
        else
            throw cRuntimeError(msg, "Unsupported message type");
    } else {
        throw cRuntimeError("Received message from invalid gate");
    }
}

void EthShim::handlePDU(PDU *pdu) {
    EV_INFO << "Doing stuff" << endl;
}

void EthShim::handleIncomingPDU(PDU *pdu) {
    EV_INFO << "Should be passed to connected IPCP" << endl;
}

void EthShim::handleIncomingArpPacket(RINArpPacket *arpPacket) {
    EV_INFO << "Sending " << arpPacket << " to arp." << endl;
    send(arpPacket, arpOut);
}

void EthShim::sendPacketToNIC(cPacket *packet) {
    EV_INFO << "Sending " << packet << " to ethernet interface." << endl;
    send(packet, ifOut);
}

inet::MACAddress EthShim::resolveApni(const APN &dstApni) const {
    Enter_Method("resolveApni(%s)", dstApni.getName().c_str());
    EV_INFO << "Received request to resolve application name " << dstApni
        << ". Passing to ARP." << endl;
    return arp->resolveAddress(dstApni);
}

void EthShim::registerApplication(const APN &apni) const {
    Enter_Method("registerApplication(%s)", apni.getName().c_str());
    EV_INFO << "Received request to register application name " << apni
        << " with ARP module." << endl;

    inet::MACAddress mac = getMacAddressOfNIC();
    arp->addStaticEntry(mac, apni);
}

const inet::MACAddress EthShim::getMacAddressOfNIC() const {
    cModule *mac = ipcProcess->getModuleByPath(".eth.mac");
    if (mac == nullptr)
        throw cRuntimeError("Unable to get address from MAC interface");
    if (!mac->hasPar("address"))
        throw cRuntimeError("MAC interface has no address parameter");
    return inet::MACAddress(mac->par("address").stringValue());
}

void EthShim::receiveSignal(cComponent *, simsignal_t signalID,
                            cObject *obj, cObject *) {
    Enter_Method_Silent();

    if (signalID == RINArp::completedRINArpResolutionSignal)
        arpResolutionCompleted(check_and_cast<RINArp::ArpNotification *>(obj));
    else if (signalID == RINArp::failedRINArpResolutionSignal)
        arpResolutionFailed(check_and_cast<RINArp::ArpNotification *>(obj));
    else
        throw cRuntimeError("Unsubscribed signalID triggered receiveSignal");
}

void EthShim::arpResolutionCompleted(RINArp::ArpNotification *entry) {
    shimFA->completedAddressResolution(entry->apName);
}

void EthShim::arpResolutionFailed(RINArp::ArpNotification *entry) {
    shimFA->failedAddressResolution(entry->apName);
}

/* How to handle delimiting? Should the delimiting module be reused, should the
 * upper layer be forced to deliver packets that are 1500 bytes long, or
 * should there be an internal delimiting module?
 */
