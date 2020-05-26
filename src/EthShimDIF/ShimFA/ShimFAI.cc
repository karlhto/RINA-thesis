#include "EthShimDIF/ShimFA/ShimFAI.h"

Define_Module(ShimFAI);

ShimFAI::ShimFAI() {
}

ShimFAI::~ShimFAI() {
}

void ShimFAI::initialize() {
}

void ShimFAI::handleMessage(cMessage *msg) {
}

bool ShimFAI::receiveAllocateRequest() {
    return true;
}
bool ShimFAI::receiveAllocateResponsePositive() {
    return true;
}
void ShimFAI::receiveAllocateResponseNegative() {
}
bool ShimFAI::receiveCreateRequest() {
    return true;
}
bool ShimFAI::receiveCreateResponsePositive(Flow* flow) {
    return true;
}
bool ShimFAI::receiveCreateResponseNegative() {
    return true;
}
bool ShimFAI::receiveDeallocateRequest() {
    return true;
}
void ShimFAI::receiveDeleteRequest(Flow* flow) {
}
void ShimFAI::receiveDeleteResponse() {
}

void ShimFAI::receiveCreateFlowResponsePositiveFromNminusOne() {
}
void ShimFAI::receiveCreateFlowResponseNegativeFromNminusOne() {
}
