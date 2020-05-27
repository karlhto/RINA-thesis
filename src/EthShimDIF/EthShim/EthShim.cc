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

#include "EthShimDIF/EthShim/EthShim.h"
#include "Common/PDU.h"

Define_Module(EthShim);

void EthShim::initialize() {
    EV << "Hello World!\n";
}

void EthShim::handleMessage(cMessage *msg) {
    /* Scenario 1: PDU from upper layer */
    /* Scenario 2: Management PDU from upper layer */
    /* Scenario 3: Request from upper relaying task */
    delete msg;
}

void EthShim::handlePDU(cMessage *msg) {
    PDU *pdu = dynamic_cast<PDU *>(msg);
}

void EthShim::handleFlowCreate(cMessage *msg) {

}

void EthShim::encapsulateFrame(cMessage *msg) {

}

/* How to handle delimiting? Should the delimiting module be reused, should the
 * upper layer be forced to deliver packets that are 1500 bytes long, or
 * should there be an internal delimiting module?
 */
