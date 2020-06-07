#pragma once

#include <omnetpp.h>
#include "DIF/FA/FAIBase.h"

class ShimFAI : public FAIBase {
  public:
    // Some public objects?

  protected:
    // Important protected objects

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

  protected:
    // cSimpleModule overrides
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};
