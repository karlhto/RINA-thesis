//
// Created by karlhto on 4/30/20.
//

#ifndef RINASIM_ETHSHIM_H
#define RINASIM_ETHSHIM_H

#include <omnetpp.h>

class EthShim : public cSimpleModule
{
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    virtual void handlePDU(cMessage *msg);
    virtual void encapsulateFrame(cMessage *msg);
    virtual void handleFlowCreate(cMessage *msg);
};

#endif //RINASIM_ETHSHIM_H
