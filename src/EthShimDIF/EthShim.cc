//
// Created by karlhto on 4/30/20.
//

#include "EthShimDIF/EthShim.h"
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
