#include "EthShimDIF/ShimFA/ShimFAI.h"
#include "Common/RINASignals.h"
#include "EthShimDIF/EthShim/EthShim.h"

Define_Module(ShimFAI);

const simsignal_t ShimFAI::ribdCreateFlowResponsePositive = registerSignal(SIG_RIBD_CreateFlowResponsePositive);

ShimFAI::ShimFAI() : localPortId(VAL_UNDEF_PORTID), remotePortId(VAL_UNDEF_PORTID) {}

ShimFAI::~ShimFAI() {}

void ShimFAI::initialize()
{
    localPortId = par(PAR_LOCALPORTID);
    remotePortId = par(PAR_REMOTEPORTID);
}

void ShimFAI::postInitialize(ShimFA *fa, Flow *flow, EthShim *shim)
{
    // Should the main storage space of the flow be FAI or nFlowTable?
    this->fa = fa;
    this->flow = flow;
    this->shim = shim;
}

void ShimFAI::handleMessage(cMessage *msg) { delete msg; }

int ShimFAI::getLocalPortId() const { return localPortId; }

// this entire module is redundant. will remove
bool ShimFAI::receiveAllocateRequest()
{
    Enter_Method("receiveAllocateRequest()");
    bool res = shim->addPort(flow->getDstApni().getApn(), localPortId);
    if (res) {
        EV << "Emitting a signal" << endl;
        emit(ribdCreateFlowResponsePositive, flow);
    }

    return res;
}

bool ShimFAI::receiveAllocateResponsePositive() { return false; }
void ShimFAI::receiveAllocateResponseNegative() {}
bool ShimFAI::receiveCreateRequest() { return false; }
bool ShimFAI::receiveCreateResponsePositive(Flow *) { return false; }
bool ShimFAI::receiveCreateResponseNegative() { return false; }
bool ShimFAI::receiveDeallocateRequest() { return false; }
void ShimFAI::receiveDeleteRequest(Flow *) {}
void ShimFAI::receiveDeleteResponse() {}

void ShimFAI::receiveCreateFlowResponsePositiveFromNminusOne() {}
void ShimFAI::receiveCreateFlowResponseNegativeFromNminusOne() {}
