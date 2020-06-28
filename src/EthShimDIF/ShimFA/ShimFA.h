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

#include "Common/QoSCube.h"
#include "DIF/FA/FABase.h"
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
class ShimFA : public FABase
{
  private:
    cModule *shimIpcProcess;
    cModule *connectedApplication;
    RINArp *arp;
    EthShim *shim;

    // Only one flow necessary
    APN registeredApplication;  ///< apName of "registered" application

    QoSCube qos;  ///< Unreliable stuff
  public:
    ShimFA();
    ~ShimFA() override;

    /** @brief Attempts to initialise new flow */
    bool receiveAllocateRequest(Flow *flow) override;
    bool receiveDeallocateRequest(Flow *flow) override;
    void completedAddressResolution(const APN &apn);
    void failedAddressResolution(const APN &apn);
    void deinstantiateFai(Flow *flow) override;
    bool createUpperFlow(const APN &apn);

    /// These are all unused in shim layer, but still implemented as required by FABase
    bool receiveMgmtAllocateRequest(Flow *mgmtflow) override;
    bool receiveMgmtAllocateRequest(APNamingInfo src, APNamingInfo dst) override;
    bool receiveMgmtAllocateFinish(APNIPair *apnip) override;
    void receiveNM1FlowCreated(Flow *flow) override;
    bool receiveCreateFlowRequestFromRibd(Flow *flow) override;
    bool invokeNewFlowRequestPolicy(Flow *flow) override;

    bool setOriginalAddresses(Flow *flow) override;
    bool setNeighborAddresses(Flow *flow) override;

  protected:
    /** @brief Initialises the QoS cube with parameters from ethernet interface */
    void initQoS();

    void setRegisteredApName();

    bool allocatePort(Flow *flow);
    void createBindings(int portID);
    void deleteBindings();

    ShimFAI *createFAI(Flow *flow);

    /// SimpleModule overrides
    void initialize(int stage) override;
    int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    void handleMessage(cMessage *msg) override;
};
