#include "EthShimDIF/ShimFA/ShimFAI.h"

#include "Common/RINASignals.h"
#include "EthShimDIF/EthShim/EthShim.h"
#include "EthShimDIF/ShimFA/ShimFA.h"
#include "DIF/FA/NFlowTable.h"
#include "DIF/FA/FANotifierBase.h"

Define_Module(ShimFAI);

const simsignal_t ShimFAI::ribdCreateFlowResponsePositive = registerSignal(SIG_RIBD_CreateFlowResponsePositive);

ShimFAI::ShimFAI() : localPortId(VAL_UNDEF_PORTID) {}

ShimFAI::~ShimFAI() {}

void ShimFAI::initialize()
{
    localPortId = static_cast<int>(par(PAR_LOCALPORTID));
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
        EV << "Successfully allocated flow" << endl;
        emit(ribdCreateFlowResponsePositive, flow);
    }

    return res;
}

bool ShimFAI::receiveCreateRequest() {
    Enter_Method("receiveCreateRequest()");
    bool res = shim->addPort(flow->getDstApni().getApn(), localPortId);
    if (res) {
        EV << "Successfully added bindings, notifying N+1" << endl;
        emit(allocateRequestSignal, flow);
    }

    return res;
}

bool ShimFAI::receiveCreateResponseNegative() { return false; }

bool ShimFAI::receiveAllocateResponsePositive()
{
    Enter_Method("receiveCreateResponsePositive()");
    EV << "Received positive allocation response! Sending pending SDUs" << endl;
    NFlowTable *nft = fa->getNFlowTable();
    NFlowTableEntry *nfte = nft->findEntryByFlow(flow);
    if (nfte == nullptr) {
        EV_ERROR << "No table entry for this flow found!" << endl;
        return false;
    }

    nft->changeAllocStatus(flow, NFlowTableEntry::TRANSFER);

    shim->sendWaitingSDUs(flow->getDstApni().getApn());
    return true;
}

void ShimFAI::receiveAllocateResponseNegative() {}


void ShimFAI::receiveSignal(cComponent *src, simsignal_t id, cObject *obj, cObject *detail)
{
    Flow *flow = check_and_cast<Flow *>(obj);
    if (flow != this->flow)
        return;

    if (id == FANotifierBase::allocateResponsePositiveSignal)
        receiveAllocateResponsePositive();
    else if (id == FANotifierBase::allocateResponseNegativeSignal)
        receiveAllocateResponseNegative();

    (void)detail;
    (void)src;
}

std::string ShimFAI::str() const {
    std::stringstream os;
    os << "FAI>" << endl
       << "\tlocal  Port-ID: " << this->localPortId << endl;
    return os.str();
}

// Irrelevant functions
bool ShimFAI::receiveCreateResponsePositive(Flow *) { return false; }
bool ShimFAI::receiveDeallocateRequest() { return false; }
void ShimFAI::receiveDeleteRequest(Flow *) {}
void ShimFAI::receiveDeleteResponse() {}

void ShimFAI::receiveCreateFlowResponsePositiveFromNminusOne() {}
void ShimFAI::receiveCreateFlowResponseNegativeFromNminusOne() {}
