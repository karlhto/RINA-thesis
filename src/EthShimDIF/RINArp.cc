#include "EthShimDIF/RINArp.h"

Define_Module(RINArp);

void RINArp::initialize() {
}

void RINArp::handleMessage(cMessage *msg) {
    delete msg;
}
