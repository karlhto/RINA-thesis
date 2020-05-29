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

Define_Module(ShimFA);

ShimFA::ShimFA() : FABase::FABase() {
    state = State::UNALLOCATED;
}

ShimFA::~ShimFA() {
}

void ShimFA::initialize() {
    cSimpleModule::initialize();

    initPointers();
    initSignals();

    // Registering an application is not supported in RINASim since any
    // upper layer connected IPCP/AP is implicitly registered. We still
    // need the IPC address/AP name of upper layer. Unfortunately pretty
    // hacky solution for the time being.
    const cGate *dstGate = ipcProcess->gate("northIo$o")->getPathEndGate();
    if (dstGate == nullptr)
        throw cRuntimeError("IPC Process lacks correct gate names");

    const cModule *conIpcProc = dstGate->getOwnerModule();
    if (!conIpcProc->hasPar("apName"))
        throw cRuntimeError("Shim IPC Process not connected to IPC process");

    std::string name = conIpcProc->par("apName").stringValue();
    APN apName(name);
    arp->registerAp(apName);
}

void ShimFA::initPointers() {
    ipcProcess = getModuleByPath(".^.^");
    arp = dynamic_cast<RINArp *>(ipcProcess->getSubmodule("arp"));
    if (arp == nullptr)
        throw cRuntimeError("Shim FA needs ARP module");

    shim = dynamic_cast<EthShim *>(ipcProcess->getSubmodule("shim"));
    if (shim == nullptr)
        throw cRuntimeError("Shim FA needs shim module");
}

void ShimFA::initSignals() {
    // We unfortunately have to handle some signals.
}


void ShimFA::handleMessage(cMessage *msg) {
    // self message is the only valid message here
    delete msg;
}

bool ShimFA::receiveAllocateRequest(Flow* flow) {
    Enter_Method("receiveAllocateRequest()");

    EV << "Received allocation request for flow with destination address "
        << flow->getDstApni().getApn() << endl;

    if (state != State::UNALLOCATED) {
        EV << "A flow is already either allocated or pending." << endl;
        return false;
    }

    state = State::ALLOCATE_PENDING;

    const auto &apName = flow->getDstApni().getApn();
    const inet::MACAddress macAddr = arp->resolveAddress(apName);

    if (macAddr != inet::MACAddress::UNSPECIFIED_ADDRESS) {
        // entry was found in cache, allocate flow at once and signal success
    }

    return true;
}

bool ShimFA::receiveDeallocateRequest(Flow* flow) {
    Enter_Method("receiveDeallocateRequest()");

    if (state != State::UNALLOCATED) {
        EV_WARN << "Deallocation requested, but no flow present." << endl;
    }

    // remove bindings

    //

    return false;
}

bool ShimFA::invokeNewFlowRequestPolicy(Flow* flow) {
    return false;
}

bool ShimFA::setOriginalAddresses(Flow* flow) {
    return false;
}

bool ShimFA::setNeighborAddresses(Flow* flow) {
    return false;
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
