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

#pragma once

#include <omnetpp.h>
#include "Common/APN.h"
#include "inet/linklayer/base/MACBase.h"
#include "inet/linklayer/common/MACAddress.h"

class RINArpPacket;

/**
 * @brief
 *
 * This module is more or less a RINA-specific version of the ARP
 * implementation done in INET.
 */
class RINArp : public cSimpleModule {
public:
    virtual const inet::MACAddress resolveAddress(const APN &addr);
    virtual bool registerAp(const APN &addr);

    class ArpCacheEntry;
    typedef std::map<APN, ArpCacheEntry *> ArpCache;

    class ArpCacheEntry
    {
    public:
        inet::MACAddress dstMac;
        bool pending = false;
        simtime_t lastUpdate;
        int numRetries = 0;
        cMessage *timer = 0;
        ArpCache::iterator myIter;
    };

    RINArp();
    virtual ~RINArp();

protected:
    simtime_t retryTimeout;
    int retryCount = 0;
    simtime_t cacheTimeout;

    ArpCache arpCache;

    // Since our ARP protocol is connected to one shim IPC process only, which
    // in turn is only connected to one interface, this works for the time
    // being
    inet::MACAddress srcMac;
    // apName of registered application process
    APN apName;

    cModule *ipcProcess;
    cModule *interface;
    cGate *netwOutGate;

    virtual void handleMessage(cMessage *msg);
    virtual void processArpPacket(RINArpPacket *arp);
    virtual void requestTimeout(cMessage *msg);
    virtual void flush();
    virtual void initiateArpResolution(ArpCacheEntry *entry);
    virtual void updateArpCache(ArpCacheEntry *entry,
                                const inet::MACAddress &addr);
    virtual bool addressRecognized(const APN &destAddr);
    virtual void sendPacketToNIC(cMessage *msg,
                                 const inet::MACAddress &macAddress,
                                 int etherType);
    virtual void sendArpRequest(const APN &addr);

    // cSimpleModule overridden
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
};
