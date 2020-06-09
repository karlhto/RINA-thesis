#include "EthShimDIF/ShimFA/ShimFAI.h"

Define_Module(ShimFAI);

ShimFAI::ShimFAI() {}

ShimFAI::~ShimFAI() {}

void ShimFAI::initialize()
{
    localPortId = par(PAR_LOCALPORTID);
    remotePortId = par(PAR_REMOTEPORTID);
}

void ShimFAI::postInitialize(FABase *fa, Flow *flow, EthShim *shim)
{
    this->fa = fa;
    this->flow = flow;
    this->shim = shim;
}

void ShimFAI::handleMessage(cMessage *msg) {}

bool ShimFAI::receiveAllocateRequest() {
    Enter_Method("receiveAllocateRequest()");

    return true;
}

bool ShimFAI::receiveAllocateResponsePositive() {}
void ShimFAI::receiveAllocateResponseNegative() {}
bool ShimFAI::receiveCreateRequest() { return false; }
bool ShimFAI::receiveCreateResponsePositive(Flow *flow) { return false; }
bool ShimFAI::receiveCreateResponseNegative() { return false; }
bool ShimFAI::receiveDeallocateRequest() { return false; }
void ShimFAI::receiveDeleteRequest(Flow *flow) {}
void ShimFAI::receiveDeleteResponse() {}

void ShimFAI::receiveCreateFlowResponsePositiveFromNminusOne() {}
void ShimFAI::receiveCreateFlowResponseNegativeFromNminusOne() {}
