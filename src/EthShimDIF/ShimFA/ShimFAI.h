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

    cMessage *creReqTimer;
    ShimFA *fa;
    EthShim *shim;
    Flow *flow;

  public:
    // Public functions
    ShimFAI();
    ~ShimFAI() override;

    std::string str() const override;

    // Need the rest of them
    bool receiveAllocateRequest() override;
    bool receiveAllocateResponsePositive() override;
    void receiveAllocateResponseNegative() override;
    bool receiveCreateRequest() override;
    bool receiveCreateResponsePositive(Flow *flow) override;
    bool receiveCreateResponseNegative() override;
    bool receiveDeallocateRequest() override;
    void receiveDeleteRequest(Flow *flow) override;
    void receiveDeleteResponse() override;

    void receiveCreateFlowResponsePositiveFromNminusOne() override;
    void receiveCreateFlowResponseNegativeFromNminusOne() override;

    void postInitialize(ShimFA *fa, Flow *flow, EthShim *shim);
    int getLocalPortId() const;

  protected:
    // cSimpleModule overrides
    void initialize() override;
    void handleMessage(cMessage *msg) override;

    // cListener override
    void receiveSignal(cComponent *src, simsignal_t id, cObject *obj, cObject *detail) override;
};
