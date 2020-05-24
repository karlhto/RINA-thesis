#ifndef RINASIM_RINARP_H
#define RINASIM_RINARP_H

#include <omnetpp.h>

#include "Common/APN.h"

class RINArpPacket;

/**
 * @brief
 *
 * This module is more or less a RINA-specific version of the ARP
 * implementation done in INET.
 */
class RINArp : public cSimpleModule {
protected:
    //ArpCache arpCache;

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
public:
    /*
    class ArpCacheEntry;
    typedef std::map<APN, ArpCacheEntry *> ArpCache;

    // TODO figure out how to store this host's interfaces
    class ArpCacheEntry
    {
    public:
        RINArp *owner = nullptr;
    };
    */
};

#endif //RINASIM_RINARP_H
