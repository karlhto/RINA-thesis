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

#pragma once

#include <omnetpp.h>
#include "Common/APN.h"
#include "inet/linklayer/common/MACAddress.h"

class PDU;
class RINArp;
class ShimFA;
class RINArpPacket;

class EthShim : public cSimpleModule, public cListener
{
  public:
    enum ShimConnectionStatus {
        UNALLOCATED,
        PENDING,
        ALLOCATED
    };

  protected:
    cGate *northIn;
    cGate *northOut;
    cGate *arpIn;
    cGate *arpOut;
    cGate *ifIn;
    cGate *ifOut;

    // Pointers to important modules
    cModule *ipcProcess;
    RINArp *arp;
    ShimFA *shimFA;

  public:
    EthShim();
    virtual ~EthShim();

    /** @brief Registers Application Naming Information with ARP */
    virtual void registerApplication(const APN &apni) const;

    /** @brief Attempts to resolve destination Application Naming Information */
    virtual inet::MACAddress resolveApni(const APN &dstApni) const;

  protected:
    virtual void initGates();
    virtual void initPointers();

    virtual void handlePDU(PDU *pdu);
    virtual void handleIncomingPDU(PDU *pdu);
    virtual void handleIncomingArpPacket(RINArpPacket *arpPacket);
    virtual void sendPacketToNIC(cPacket *packet);

    const virtual inet::MACAddress getMacAddressOfNIC() const;

    /// cSimpleModule overrides
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    /// cListener overrides, for ARP signals
    virtual void receiveSignal(cComponent *source, simsignal_t signalID,
                               cObject *obj, cObject *details) override;
};
