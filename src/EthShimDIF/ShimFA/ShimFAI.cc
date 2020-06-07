#include "EthShimDIF/ShimFA/ShimFAI.h"

ShimFAI::ShimFAI() {}

ShimFAI::~ShimFAI() {}

void ShimFAI::initialize() {
}

void ShimFAI::handleMessage(cMessage *msg) {
}

bool ShimFAI::receiveAllocateRequest() {
}
bool ShimFAI::receiveAllocateResponsePositive() {
}
void ShimFAI::receiveAllocateResponseNegative() {
}
bool ShimFAI::receiveCreateRequest() {
    return false;
}
bool ShimFAI::receiveCreateResponsePositive(Flow* flow) {
    return false;
}
bool ShimFAI::receiveCreateResponseNegative() {
}
bool ShimFAI::receiveDeallocateRequest() {
}
void ShimFAI::receiveDeleteRequest(Flow* flow) {
}
void ShimFAI::receiveDeleteResponse() {
}

void ShimFAI::receiveCreateFlowResponsePositiveFromNminusOne() {
}
void ShimFAI::receiveCreateFlowResponseNegativeFromNminusOne() {
}
