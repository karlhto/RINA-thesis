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
#include "EthShimDIF/RINArp/RINArpPacket_m.h"
#include "Common/PDU.h"

Define_Module(EthShim);

EthShim::EthShim() {
}

EthShim::~EthShim() {
}

/**
 * @brief Initialises gates and pointers
 */
void EthShim::initialize() {
    initPointers();
    initGates();
}

/**
 * @brief Initialises pointers
 */
void EthShim::initPointers() {
    ipcProcess = getModuleByPath(".^.^");
    arp = ipcProcess->getSubmodule("arp");
}

/**
 * @brief Initialises gates
 */
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
    EV_INFO << "Sending " << packet << " to output interface." << endl;
    send(packet, ifOut);
}

/* How to handle delimiting? Should the delimiting module be reused, should the
 * upper layer be forced to deliver packets that are 1500 bytes long, or
 * should there be an internal delimiting module?
 */
