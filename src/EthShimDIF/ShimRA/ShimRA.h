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

#include "DIF/RA/RABase.h"

class ShimRA : public RABase
{
  protected:
  public:
    ShimRA();
    ~ShimRA() override;

    void createNM1Flow(Flow *flow) override;
    void createNM1FlowWithoutAllocate(Flow *flow) override;
    void createNFlow(Flow *flow) override;
    void postNFlowAllocation(Flow *flow) override;
    void postNM1FlowAllocation(NM1FlowTableItem *ftItem) override;
    void removeNM1Flow(Flow *flow) override;
    void removeNM1FlowBindings(NM1FlowTableItem *ftItem) override;
    bool bindNFlowToNM1Flow(Flow *flow) override;
    void blockNM1PortOutput(NM1FlowTableItem *ftItem) override;
    void unblockNM1PortOutput(NM1FlowTableItem *ftItem) override;
    NM1FlowTable *getFlowTable() override;
    void signalizeSlowdownRequestToRIBd(cPacket *pdu) override;
    void signalizeSlowdownRequestToEFCP(cObject *pdu) override;
    bool hasFlow(const std::string &addr, const std::string &qosId) override;

  protected:
    /// cSimpleModule overrides
    void handleMessage(cMessage *msg) override;
    void initialize(int stage) override;
    int numInitStages() const override { return 2; }
};
