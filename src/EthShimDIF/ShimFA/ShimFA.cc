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
#include "inet/linklayer/common/MACAddress.h"

Define_Module(ShimFA);

ShimFA::ShimFA() : FABase::FABase(), state(ShimConnectionState::UNALLOCATED) {}

ShimFA::~ShimFA() {
}

void ShimFA::initialize(int stage) {
    cSimpleModule::initialize(stage);

    if (stage == 0) {
        initPointers();
        initSignals();
    }
    else if (stage == 1) {
        // Needs to be done in initialisation phase since registration is implicit
        // in RINASim
        setRegisteredApName();
    }
}

void ShimFA::initPointers() {
    shimIpcProcess = getModuleByPath("^.^");
    arp = dynamic_cast<RINArp *>(shimIpcProcess->getSubmodule("arp"));
    if (arp == nullptr)
        throw cRuntimeError("Shim FA needs ARP module");

    shim = dynamic_cast<EthShim *>(shimIpcProcess->getSubmodule("shim"));
    if (shim == nullptr)
        throw cRuntimeError("Shim FA needs shim module");

    // Registering an application is not supported in RINASim since any upper
    // layer connected IPCP/AP is implicitly registered. We still need the AP
    // name of the upper layer. Unfortunately pretty hacky solution for the
    // time being, but finds connected IPC process.
    const cGate *dstGate = shimIpcProcess->gate("northIo$o")->getPathEndGate();
    if (dstGate == nullptr)
        throw cRuntimeError("IPC Process lacks correct gate names");

    connectedApplication = dstGate->getOwnerModule();
    if (!connectedApplication->hasPar("apName"))
        throw cRuntimeError("Shim IPC Process not connected to IPC process");
}

void ShimFA::initSignals() {
    // We unfortunately have to handle some signals.
}

void ShimFA::setRegisteredApName() {
    std::string name = connectedApplication->par("apName").stringValue();
    registeredApplication = APN(name);

    // Registers application with static entry in ARP
    shim->registerApplication(registeredApplication);
}


void ShimFA::handleMessage(cMessage *msg) {
    // self message is the only valid message here
    delete msg;
}

bool ShimFA::receiveAllocateRequest(Flow* flow) {
    Enter_Method("receiveAllocateRequest()");

    EV << "Received allocation request for flow with destination address "
        << flow->getDstApni().getApn() << endl;

    if (state != ShimConnectionState::UNALLOCATED) {
        EV << "A flow is already either allocated or pending." << endl;
        return false;
    }

    const auto &apName = flow->getDstApni().getApn();
    const inet::MACAddress macAddr = arp->resolveAddress(apName);

    // TODO implement QoS validation
    //validateQosRequirements(flow);

    if (macAddr != inet::MACAddress::UNSPECIFIED_ADDRESS) {
        // entry was found in cache, allocate flow at once and signal success
        state = ShimConnectionState::ALLOCATED;
        createBindings();
        return true;
    }

    state = ShimConnectionState::ALLOCATE_PENDING;

    return true;
}

bool ShimFA::receiveDeallocateRequest(Flow* flow) {
    Enter_Method("receiveDeallocateRequest()");

    EV << "Received deallocation request for flow with destination APN " << endl;

    if (state == ShimConnectionState::UNALLOCATED) {
        EV_WARN << "Deallocation requested, but no flow present." << endl;
        return false;
    }

    // remove bindings

    return false;
}

void ShimFA::receiveArpUpdate(const APN &dstApn) {
}

// Not sure what to do with this function as of yet. This is called by upper
// layer, but not checked. It's possible at least a subset of the flow
// allocation policies should be implemented
bool ShimFA::invokeNewFlowRequestPolicy(Flow* flow) {
    return true;
}

bool ShimFA::setOriginalAddresses(Flow* flow) {
    return false;
}

bool ShimFA::setNeighborAddresses(Flow* flow) {
    return false;
}

void ShimFA::createBindings() {
}

void ShimFA::deleteBindings() {
}

void ShimFA::receiveSignal(cComponent *source, simsignal_t signalID,
                           cObject *obj, cObject *details) {
}


/* Mandatory function implementations */
bool ShimFA::receiveMgmtAllocateRequest(Flow*) {
    throw cRuntimeError("ShimFA does not support creation of management flows");
}

bool ShimFA::receiveMgmtAllocateRequest(APNamingInfo, APNamingInfo) {
    throw cRuntimeError("ShimFA does not support creation of management flows");
}

bool ShimFA::receiveMgmtAllocateFinish() {
    throw cRuntimeError("ShimFA does not support creation of management flows");
}

void ShimFA::receiveNM1FlowCreated(Flow*) {
    throw cRuntimeError("ShimFA should not be on medium");
}

bool ShimFA::receiveCreateFlowRequestFromRibd(Flow*) {
    throw cRuntimeError("ShimFA should not need to communicate with RIBd");
}

void ShimFA::deinstantiateFai(Flow*) {
    throw cRuntimeError("ShimFA should not utilise FAI");
}
