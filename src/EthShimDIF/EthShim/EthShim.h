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
#include <memory>
#include <queue>

#include "Common/APN.h"
#include "EthShimDIF/RINArp/RINArp.h"
#include "inet/linklayer/common/MACAddress.h"
#include "inet/networklayer/common/InterfaceEntry.h"

class RINArpPacket;
class ShimFA;
class SDUData;

class EthShim : public cSimpleModule, public cListener
{
  private:
    enum ConnState {
        PENDING,
        ALLOCATED
    };

    struct ShimEntry;
    using FlowMap = std::map<APN, std::unique_ptr<ShimEntry>>;

    struct ShimEntry {
        ConnState state;
        std::queue<SDUData *> outQueue;
        std::queue<SDUData *> inQueue;
        cGate *gate;
        FlowMap::iterator myIter;
    };

    FlowMap flows;

    // Pointers to important modules
    cModule *ipcProcess;
    RINArp *arp;
    ShimFA *shimFA;
    inet::InterfaceEntry *ie;

  public:
    EthShim();
    ~EthShim() override;

    /** @brief Registers Application Naming Information with Arp */
    void registerApplication(const APN &apni) const;

    /** @brief Adds mapping and creates bindings for a flow */
    bool addPort(const APN &dstApn, const int &portId);

    /** @brief Sends waiting SDUs in queue */
    void sendWaitingSDUs(const APN &srcApn);

    /** @brief Attempt to resolve specified address using ARP */
    bool createEntry(const APN &dstApn);

  protected:
    void handleSDU(SDUData *sdu, cGate *gate);
    void handleIncomingSDU(SDUData *sdu);
    void handleIncomingArpPacket(RINArpPacket *arpPacket);
    void sendPacketToNIC(cMessage *msg);

    void insertOutgoingSDU(const APN &dstApn, SDUData *sdu);
    void insertIncomingSDU(const APN &srcApn, SDUData *sdu);

    void arpResolutionCompleted(RINArp::ArpNotification *entry);
    void arpResolutionFailed(RINArp::ArpNotification *entry);

    /// cSimpleModule overrides
    void initialize(int stage) override;
    int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    void handleMessage(cMessage *msg) override;

    /// cListener overrides, for Arp signals
    void receiveSignal(cComponent *source,
                       simsignal_t signalID,
                       cObject *obj,
                       cObject *details) override;
};
