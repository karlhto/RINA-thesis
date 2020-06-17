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
#include "EthShimDIF/RINArp/RINArp.h"
#include "inet/linklayer/common/MACAddress.h"
#include "inet/networklayer/common/InterfaceEntry.h"

class SDUData;
class ShimFA;
class RINArpPacket;

class EthShim : public cSimpleModule, public cListener
{
  protected:
    std::map<cGate *, APN> gateMap;
    std::map<cGate *, APN>::iterator gateMapIt;
    std::map<APN, std::vector<SDUData *>> queue;
    std::map<APN, std::vector<SDUData *>>::iterator queueIt;

    // Pointers to important modules
    cModule *ipcProcess;
    RINArp *arp;
    ShimFA *shimFA;
    inet::InterfaceEntry *ie;

  public:
    EthShim();
    virtual ~EthShim();

    /** @brief Registers Application Naming Information with Arp */
    virtual void registerApplication(const APN &apni) const;

    /** @brief Adds mapping and creates bindings for a flow */
    virtual bool addPort(const APN &dstApn, int portId);

    /** @brief Sends waiting SDUs in queue */
    virtual void sendWaitingSDUs(const APN &srcApn);

  protected:
    void handleSDU(SDUData *sdu, cGate *gate);
    void handleIncomingSDU(SDUData *sdu);
    void handleIncomingArpPacket(RINArpPacket *arpPacket);
    void sendPacketToNIC(cPacket *packet);

    void insertSDU(SDUData *sdu, const APN &srcApn);

    void arpResolutionCompleted(RINArp::ArpNotification *entry);
    void arpResolutionFailed(RINArp::ArpNotification *entry);

    /// cSimpleModule overrides
    virtual void initialize(int stage) override;
    virtual int numInitStages() const { return inet::NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;

    /// cListener overrides, for Arp signals
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj,
                               cObject *details) override;
};
