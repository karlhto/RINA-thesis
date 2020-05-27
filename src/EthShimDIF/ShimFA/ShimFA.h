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

class ShimFA : public FABase {
public:
    ShimFA();
    virtual ~ShimFA();

    virtual bool receiveAllocateRequest(Flow* flow);
    virtual bool receiveMgmtAllocateRequest(Flow* mgmtflow);
    virtual bool receiveMgmtAllocateRequest(APNamingInfo src, APNamingInfo dst);
    virtual bool receiveMgmtAllocateFinish();
    virtual void receiveNM1FlowCreated(Flow* flow);
    virtual bool receiveCreateFlowRequestFromRibd(Flow* flow);
    virtual bool receiveDeallocateRequest(Flow* flow);
    virtual void deinstantiateFai(Flow* flow);
    virtual bool invokeNewFlowRequestPolicy(Flow* flow);

    virtual bool setOriginalAddresses(Flow* flow);
    virtual bool setNeighborAddresses(Flow* flow);

    NFlowTable* getNFlowTable() const;
    const Address& getMyAddress() const;

protected:
    NFlowTable* N_flowTable;
    Address MyAddress;

    //SimpleModule overloads
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    void initMyAddress();
};
