#pragma once

#include <omnetpp.h>

#include "DIF/FA/FAIBase.h"

class ShimFA;
class EthShim;

class ShimFAI : public FAIBase, public cListener
{
  public:
    // Some public objects?
    enum ConnectionState { UNALLOCATED, ALLOCATE_PENDING, ALLOCATED };

    // oh god why
    static const simsignal_t ribdCreateFlowResponsePositive;

  protected:
    // Important protected objects
    int localPortId;
    int remotePortId;

    cMessage *creReqTimer;
    ShimFA *fa;
    EthShim *shim;
    Flow *flow;

  public:
    // Public functions
    ShimFAI();
    virtual ~ShimFAI();

    // Need the rest of them
    virtual bool receiveAllocateRequest();
    virtual bool receiveAllocateResponsePositive();
    virtual void receiveAllocateResponseNegative();
    virtual bool receiveCreateRequest();
    virtual bool receiveCreateResponsePositive(Flow *flow);
    virtual bool receiveCreateResponseNegative();
    virtual bool receiveDeallocateRequest();
    virtual void receiveDeleteRequest(Flow *flow);
    virtual void receiveDeleteResponse();

    virtual void receiveCreateFlowResponsePositiveFromNminusOne();
    virtual void receiveCreateFlowResponseNegativeFromNminusOne();

    void postInitialize(ShimFA *fa, Flow *flow, EthShim *shim);
    int getLocalPortId() const;

  protected:
    // cSimpleModule overrides
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    // cListener override
    virtual void receiveSignal(cComponent *src, simsignal_t id, cObject *obj, cObject *detail);
};
