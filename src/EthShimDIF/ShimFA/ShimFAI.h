#pragma once

#include <omnetpp.h>
#include "DIF/FA/FAIBase.h"

class FABase;
class EthShim;

class ShimFAI : public FAIBase {
  public:
    // Some public objects?
    enum ConnectionState { UNALLOCATED, ALLOCATE_PENDING, ALLOCATED };

  protected:
    // Important protected objects
    int localPortId;
    int remotePortId;

    cMessage *creReqTimer;
    FABase* fa;
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
    virtual bool receiveCreateResponsePositive(Flow* flow);
    virtual bool receiveCreateResponseNegative();
    virtual bool receiveDeallocateRequest();
    virtual void receiveDeleteRequest(Flow* flow);
    virtual void receiveDeleteResponse();

    virtual void receiveCreateFlowResponsePositiveFromNminusOne();
    virtual void receiveCreateFlowResponseNegativeFromNminusOne();

    void postInitialize(FABase *fa, Flow *flow, EthShim *shim);

  protected:
    // cSimpleModule overrides
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};
