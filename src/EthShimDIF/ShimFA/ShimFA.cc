// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Brno University of Technology, PRISTINE project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "EthShimDIF/ShimFA/ShimFA.h"

#include "Common/RINASignals.h"
#include "EthShimDIF/ShimFA/ShimFAI.h"
#include "EthShimDIF/EthShim/EthShim.h"
#include "EthShimDIF/RINArp/RINArp.h"
#include "inet/linklayer/common/MACAddress.h"

Define_Module(ShimFA);

/*
 * Initialisation functionality
 */
ShimFA::ShimFA() : FABase::FABase(), resolving(false), qos(QoSCube())
{
    qos.setQosId(VAL_ANYQOSID);
}

ShimFA::~ShimFA() {}

void ShimFA::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        shimIpcProcess = getModuleByPath("^.^");
        if (shimIpcProcess == nullptr)
            throw cRuntimeError("ShimFA not used in any IPC Process");

        arp = getRINAModule<RINArp *>(this, 2, {"arp"});
        shim = getRINAModule<EthShim *>(this, 2, {"shim"});
        nFlowTable = getRINAModule<NFlowTable *>(this, 1, {MOD_NFLOWTABLE});

        // Registering an application is not supported in RINASim since any upper
        // layer connected IPCP/AP is implicitly registered. We still need the AP name
        // of the upper layer. Unfortunately pretty hacky solution for the time being,
        // but finds connected IPC process
        const cGate *dstGate = shimIpcProcess->gate("northIo$o")->getPathEndGate();
        if (dstGate == nullptr) {
            EV_ERROR
                << "Shim IPC not connected to overlying application. It will be able to receive "
                << "ARP requests, but will never send ARP reply" << endl;
            return;
        }

        connectedApplication = dstGate->getOwnerModule();
        if (!connectedApplication->hasPar("apName"))
            throw cRuntimeError(
                "Shim IPC process not connected to overlying IPC/Application Process");

        // Sets up listening for this module
        arp->subscribe(RINArp::completedRINArpResolutionSignal, this);
        arp->subscribe(RINArp::failedRINArpResolutionSignal, this);
    } else if (stage == inet::INITSTAGE_NETWORK_LAYER_2) {
        // Needs to be done in initialisation phase since registration is implicit in RINASim.
        // TODO look for alternative function
        setRegisteredApName();

        if (shimIpcProcess != nullptr) {
            // FIXME: Should probably add an API call to formally register application
            // Registers application with static entry in Arp, needs to be called after stage 0 to
            // guarantee allocation of MAC address
            shim->registerApplication(registeredApplication);
        }
    }
}

void ShimFA::setRegisteredApName()
{
    std::string name = connectedApplication->par("apName").stringValue();
    registeredApplication = APN(name);
}

void ShimFA::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage() && opp_strcmp(msg->getName(), TIM_FAPENDFLOWS) == 0) {

    }
    // self message is the only valid message here
    delete msg;
}

bool ShimFA::createUpperFlow(const APN &dstApn) {
    Enter_Method("createUpperFlow(%s)", dstApn.c_str());

    EV << "Received request to forward allocation request to N+1, with source address " << dstApn
       << endl;

    ConnectionId connId;
    connId.setQoSId(VAL_ANYQOSID);

    Flow *flow = new Flow(registeredApplication, dstApn);
    flow->setQosCube(qos);
    flow->setConId(connId);
    flow->setDstNeighbor(dstApn);
    flow->setSrcNeighbor(registeredApplication);

    ShimFAI *fai = createFAI(flow);
    nFlowTable->insertNew(flow);
    flow->setSrcPortId(fai->getLocalPortId());
    nFlowTable->setFaiToFlow(fai, flow);
    nFlowTable->changeAllocStatus(flow, NFlowTableEntry::TRANSFER);

    return fai->receiveCreateRequest();
}

bool ShimFA::receiveAllocateRequest(Flow *flow)
{
    Enter_Method("receiveAllocateRequest()");

    EV << "Received allocation request for flow with destination address "
       << flow->getDstApni().getApn() << endl;

    const auto &apName = flow->getDstApni().getApn();
    auto nft = nFlowTable->findEntryByApnisAndQosId(registeredApplication, apName, qos.getQosId());
    if (nft != nullptr) {
        ShimFAI *fai = static_cast<ShimFAI *>(nft->getFai());
        flow->setSrcPortId(fai->getLocalPortId());
        //fai->receiveAllocateRequest(flow);
        return true;
    }

    ConnectionId connId;
    connId.setQoSId(VAL_ANYQOSID);
    flow->setConId(connId);
    flow->setQosCube(qos);

    ShimFAI *fai = createFAI(flow);
    nFlowTable->insertNew(flow);
    flow->setSrcPortId(fai->getLocalPortId());
    nFlowTable->setFaiToFlow(fai, flow);
    nFlowTable->changeAllocStatus(flow, NFlowTableEntry::ALLOC_PEND);

    const inet::MACAddress macAddr = arp->resolveAddress(apName);

    // TODO implement some form of QoS checking
    // validateQosRequirements(flow);

    if (macAddr != inet::MACAddress::UNSPECIFIED_ADDRESS)
        return fai->receiveAllocateRequest();

    resolving = true;
    return true;
}

bool ShimFA::receiveDeallocateRequest(Flow *flow)
{
    (void)flow;
    Enter_Method("receiveDeallocateRequest()");
    EV << "Received deallocation request for flow with destination APN " << endl;

    //fai->receiveDeallocateRequest();

    // Check state of FAI
    // remove bindings

    return false;
}

void ShimFA::completedAddressResolution(const APN &dstApn)
{
    Enter_Method("completedAddressResolution(%s)", dstApn.getName().c_str());
    EV << "Completed address resolution for " << dstApn << endl;
    auto nft = nFlowTable->findEntryByApnisAndQosId(registeredApplication, dstApn, VAL_ANYQOSID);
    if (nft == nullptr) {
        EV << "No such pending flow found" << endl;
        return;
    } else if (nft->getAllocateStatus() == NFlowTableEntry::TRANSFER) {
        EV << "Flow already exists, continue sending" << endl;
    }

    resolving = false;
    Flow *flow = nft->getFlow();
    nFlowTable->changeAllocStatus(flow, NFlowTableEntry::TRANSFER);
    nft->getFai()->receiveAllocateRequest();
}

void ShimFA::failedAddressResolution(const APN &dstApn)
{
    Enter_Method("failedAddressResolution(%s)", dstApn.getName().c_str());
    resolving = false;
    auto nft = nFlowTable->findEntryByApnisAndQosId(registeredApplication, dstApn, VAL_ANYQOSID);
    ShimFAI *fai = static_cast<ShimFAI *>(nft->getFai());
    fai->receiveDeallocateRequest();
    // something something FAI stop createresponsenegative
}

ShimFAI *ShimFA::createFAI(Flow *flow)
{
    cModuleType *type = cModuleType::get("rina.src.EthShimDIF.ShimFA.ShimFAI");
    int portId = getEnvir()->getRNG(0)->intRand(USHRT_MAX);

    std::ostringstream ostr;
    ostr << "fai_" << portId;

    // Instantiate module
    ShimFAI *fai =
        check_and_cast<ShimFAI *>(type->createScheduleInit(ostr.str().c_str(), getParentModule()));
    fai->par(PAR_LOCALPORTID) = portId;
    fai->postInitialize(this, flow, shim);
    return fai;
}

// Not sure what to do with this function as of yet. This is called by upper
// layer, but not checked. It's possible at least a subset of the flow
// allocation policies should be implemented
bool ShimFA::invokeNewFlowRequestPolicy(Flow *) { return true; }

bool ShimFA::setOriginalAddresses(Flow *) { return false; }

bool ShimFA::setNeighborAddresses(Flow *) { return false; }

bool ShimFA::allocatePort(Flow *) { return false; }

void ShimFA::createBindings(int) {}

void ShimFA::deleteBindings() {}

void ShimFA::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details)
{
    if (!resolving)
        return;

    RINArp::ArpNotification *notification = check_and_cast<RINArp::ArpNotification *>(obj);
    if (signalID == RINArp::completedRINArpResolutionSignal)
        completedAddressResolution(notification->apName);
    else if (signalID == RINArp::failedRINArpResolutionSignal)
        failedAddressResolution(notification->apName);
    else
        throw cRuntimeError("Unsubscribed signalID triggered receiveSignal");

    (void)source;
    (void)details;
}

/* Mandatory function implementations */
bool ShimFA::receiveMgmtAllocateRequest(Flow *)
{
    throw cRuntimeError("ShimFA does not support creation of management flows");
}

bool ShimFA::receiveMgmtAllocateRequest(APNamingInfo, APNamingInfo)
{
    throw cRuntimeError("ShimFA does not support creation of management flows");
}

bool ShimFA::receiveMgmtAllocateFinish(APNIPair *)
{
    throw cRuntimeError("ShimFA does not support creation of management flows");
}

void ShimFA::receiveNM1FlowCreated(Flow *)
{
    throw cRuntimeError("ShimFA should not be on medium");
}

bool ShimFA::receiveCreateFlowRequestFromRibd(Flow *)
{
    throw cRuntimeError("ShimFA should not need to communicate with RIBd");
}

void ShimFA::deinstantiateFai(Flow *) { throw cRuntimeError("ShimFA should not utilise FAI"); }
