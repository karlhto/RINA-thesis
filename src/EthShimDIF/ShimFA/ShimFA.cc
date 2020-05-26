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

ShimFA::ShimFA() {
    this->N_flowTable = NULL;
}

ShimFA::~ShimFA() {
    this->N_flowTable = NULL;
}

void ShimFA::initialize() {
    // initialise signals
}

void ShimFA::handleMessage(cMessage *msg) {
    // self message is the only valid message here
    delete msg;
}

NFlowTable* ShimFA::getNFlowTable() const {
    return N_flowTable;
}

void ShimFA::initMyAddress() {
    //Setup MyAddress
    cModule* ipc = this->getModuleByPath("^.^");
    MyAddress = Address(ipc->par(PAR_IPCADDR), ipc->par(PAR_DIFNAME));
    EV << "SrcAddress that this FA will use is " << MyAddress << endl;

    std::ostringstream description;
    description << "address: " << MyAddress.getIpcAddress()
                << "\ndif: " << MyAddress.getDifName();
    ipc->getDisplayString().setTagArg("t", 0, description.str().c_str());
    ipc->getDisplayString().setTagArg("t", 1, "r");
}


const Address& ShimFA::getMyAddress() const {
    return MyAddress;
}

bool ShimFA::receiveAllocateRequest(Flow* flow) {
    return false;
}
bool ShimFA::receiveMgmtAllocateRequest(Flow* mgmtflow) {
    return false;
}
bool ShimFA::receiveMgmtAllocateRequest(APNamingInfo src, APNamingInfo dst) {
    return false;
}
bool ShimFA::receiveMgmtAllocateFinish() {
    return false;
}
void ShimFA::receiveNM1FlowCreated(Flow* flow) {
}
bool ShimFA::receiveCreateFlowRequestFromRibd(Flow* flow) {
    return false;
}
bool ShimFA::receiveDeallocateRequest(Flow* flow) {
    return false;
}
void ShimFA::deinstantiateFai(Flow* flow) {
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
