#ifndef RINASIM_RINARP_H
#define RINASIM_RINARP_H

#include <omnetpp.h>

#include "Common/APN.h"


/**
 * @brief
 *
 * This module is more or less a RINA-specific version of the ARP
 * implementation done in INET.
 */
class RINArp : public cSimpleModule {
public:
    class ARPCacheEntry;
    typedef std::map<APN, ARPCacheEntry *> ARPCache;

    // TODO figure out how to store this host's interfaces
    class ARPCacheEntry
    {
    public:
        RINArp *owner = nullptr;
    };
};

#endif //RINASIM_RINARP_H
