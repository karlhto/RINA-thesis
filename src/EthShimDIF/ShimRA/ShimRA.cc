#include "EthShimDIF/ShimRA/ShimRA.h"

Define_Module(ShimRA);

ShimRA::ShimRA() = default;

ShimRA::~ShimRA() = default;

void ShimRA::initialize(int stage)
{
    RABase::initialize(stage);
}

void ShimRA::handleMessage(cMessage *msg)
{
    delete msg;
    throw cRuntimeError("This module is not supposed to handle messages");
}

void ShimRA::createNM1Flow(Flow *flow)
{
    (void)flow;
}

void ShimRA::createNM1FlowWithoutAllocate(Flow *flow)
{
    (void)flow;
}

void ShimRA::createNFlow(Flow *flow)
{
    (void)flow;
}

void ShimRA::postNFlowAllocation(Flow *flow)
{
    (void)flow;
}

void ShimRA::postNM1FlowAllocation(NM1FlowTableItem *ftItem)
{
    (void)ftItem;
}

void ShimRA::removeNM1Flow(Flow *flow)
{
    (void)flow;
}

void ShimRA::removeNM1FlowBindings(NM1FlowTableItem *ftItem)
{
    (void)ftItem;
}

bool ShimRA::bindNFlowToNM1Flow(Flow *flow)
{
    (void)flow;
    return false;
}

void ShimRA::blockNM1PortOutput(NM1FlowTableItem *ftItem)
{
    (void)ftItem;
}

void ShimRA::unblockNM1PortOutput(NM1FlowTableItem *ftItem)
{
    (void)ftItem;
}

NM1FlowTable *ShimRA::getFlowTable()
{
    return nullptr;
}

void ShimRA::signalizeSlowdownRequestToRIBd(cPacket *pdu)
{
    (void)pdu;
}

void ShimRA::signalizeSlowdownRequestToEFCP(cObject *pdu)
{
    (void)pdu;
}

bool ShimRA::hasFlow(const std::string &addr, const std::string &qosId)
{
    (void)addr;
    (void)qosId;
    return false;
}
