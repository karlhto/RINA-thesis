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

#ifndef RABASE_H_
#define RABASE_H_

#include <omnetpp.h>

#include "Common/QoSCube.h"

//Consts
extern const char* PAR_QOSDATA;
extern const char* ELEM_QOSCUBE;
extern const char* PAR_QOSREQ;
extern const char* ELEM_QOSREQ;
extern const char* ATTR_ID;

class Flow;
class NM1FlowTable;
class NM1FlowTableItem;

typedef std::list<QoSCube> QoSCubeSet;
typedef QoSCubeSet::const_iterator QCubeCItem;

class RABase : public cSimpleModule
{
  public:
    static const simsignal_t createFlowPositiveSignal;
    static const simsignal_t createFlowNegativeSignal;
    static const simsignal_t invokeSlowdownSignal;
    static const simsignal_t executeSlowdownSignal;

  protected:
    QoSCubeSet QoSCubes;

  public:
    RABase();
    virtual ~RABase();

    virtual void createNM1Flow(Flow *flow) = 0;
    virtual void createNM1FlowWithoutAllocate(Flow *flow) = 0;
    virtual void createNFlow(Flow *flow) = 0;
    virtual void postNFlowAllocation(Flow* flow) = 0;
    virtual void postNM1FlowAllocation(NM1FlowTableItem* ftItem) = 0;
    virtual void removeNM1Flow(Flow *flow) = 0;
    virtual void removeNM1FlowBindings(NM1FlowTableItem* ftItem) = 0;
    virtual bool bindNFlowToNM1Flow(Flow* flow) = 0;
    virtual void blockNM1PortOutput(NM1FlowTableItem* ftItem) = 0;
    virtual void unblockNM1PortOutput(NM1FlowTableItem* ftItem) = 0;
    virtual NM1FlowTable* getFlowTable() = 0;
    virtual void signalizeSlowdownRequestToRIBd(cPacket* pdu) = 0;
    virtual void signalizeSlowdownRequestToEFCP(cObject* pdu) = 0;
    virtual bool hasFlow(std::string addr, std::string qosId) = 0;

    const QoSCubeSet& getQoSCubes() const;
    const QoSCube* getQoSCubeById(std::string qosId) const;

  protected:
    virtual void initQoSCubes();

    //SimpleModule overloads
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return 1; };
    virtual void handleMessage(cMessage *msg) override;
};

//Free function
std::ostream& operator<< (std::ostream& os, const QoSCubeSet& cubes);

#endif /* RABASE_H_ */
