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

#include "DIF/FA/FABase.h"
#include "Common/QoSCube.h"
#include "inet/common/InitStages.h"

class ShimFAI;
class EthShim;
class RINArp;

/**
 * Shim specific Flow Allocator.
 *
 * Different from normal IPC Process Flow Allocator in the way that no flow allocator instances are
 * actually allocated, only the first registered flow is.
 */
class ShimFA : public FABase, public cListener
{
  protected:
    cModule *shimIpcProcess;
    cModule *connectedApplication;
    RINArp *arp;
    EthShim *shim;
    bool resolving;

    // Only one flow necessary
    APN registeredApplication;  ///< apName of "registered" application

    QoSCube qos; ///< Unreliable stuff
  public:
    ShimFA();
    virtual ~ShimFA();

    /** @brief Attempts to initialise new flow */
    virtual bool receiveAllocateRequest(Flow *flow);
    virtual bool receiveDeallocateRequest(Flow *flow);
    virtual void completedAddressResolution(const APN &apn);
    virtual void failedAddressResolution(const APN &apn);
    virtual void deinstantiateFai(Flow *flow);
    virtual bool createUpperFlow(const APN &apn);

    /// These are all unused in shim layer, but still implemented
    virtual bool receiveMgmtAllocateRequest(Flow *mgmtflow);
    virtual bool receiveMgmtAllocateRequest(APNamingInfo src, APNamingInfo dst);
    virtual bool receiveMgmtAllocateFinish(APNIPair *apnip);
    virtual void receiveNM1FlowCreated(Flow *flow);
    virtual bool receiveCreateFlowRequestFromRibd(Flow *flow);
    virtual bool invokeNewFlowRequestPolicy(Flow *flow);

    virtual bool setOriginalAddresses(Flow *flow);
    virtual bool setNeighborAddresses(Flow *flow);

  protected:
    /** @brief Initialises the QoS cube with parameters from ethernet interface */
    virtual void initQoS();

    virtual void setRegisteredApName();

    virtual bool allocatePort(Flow *flow);
    virtual void createBindings(int portID);
    virtual void deleteBindings();

    ShimFAI *createFAI(Flow *flow);

    /// SimpleModule overrides
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;

    /// cListener overrides
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj,
                               cObject *details) override;
};
