//
// Created by karlhto on 4/30/20.
//

#ifndef RINASIM_ETHSHIM_H
#define RINASIM_ETHSHIM_H

#include <omnetpp.h>
#include "inet/linklayer/ethernet/EtherMAC.h"

#define GATE_SHIM_NORTHIO "northIo"
#define GATE_SHIM_SOUTHIO "southIo"

class EthShim : public cSimpleModule
{
public:
    EthShim() {};
    virtual ~EthShim();

    inet::EtherMAC *getMac() const;

private:
    inet::EtherMAC *mac;

protected:
    virtual void initialize(int step);
};

#endif //RINASIM_SHIM_H
