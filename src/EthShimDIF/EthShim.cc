//
// Created by karlhto on 4/30/20.
//

#include "EthShim.h"

Define_Module(EthShim);

void EthShim::initialize(int step) {
}

inet::EtherMAC *EthShim::getMac() const {
    return mac;
}
