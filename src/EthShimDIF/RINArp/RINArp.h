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
#include "inet/linklayer/common/MACAddress.h"

class RINArpPacket;

/**
 * RINArp
 *
 * This module is more or less a RINA-specific version of the ARP
 * implementation done in INET.
 */
class RINArp : public cSimpleModule
{
  public:
    class ArpCacheEntry;
    typedef std::map<APN, ArpCacheEntry *> ArpCache;

    /**
     * Used for resolving path for APNs
     */
    class ArpCacheEntry
    {
      public:
        inet::MACAddress macAddress;
        bool pending = false;
        simtime_t lastUpdate;
        int numRetries = 0;
        cMessage *timer = nullptr;
        ArpCache::iterator myIter;
    };

    /**
     * Notifications for changes in ARP, like a completed ARP resolution
     */
    class ArpNotification : public cObject
    {
      public:
        APN apName;
        inet::MACAddress macAddress;

      public:
        ArpNotification(APN apName, inet::MACAddress macAddress)
            : apName(apName), macAddress(macAddress)
        {
        }
    };

    /** @brief Signals for publishing ARP state changes */
    static const simsignal_t initiatedRINArpResolutionSignal;
    static const simsignal_t completedRINArpResolutionSignal;
    static const simsignal_t failedRINArpResolutionSignal;

  protected:
    /** @brief Timeouts to about retries, with max timeout retryCount */
    simtime_t retryTimeout;
    int retryCount = 0;

    /** @brief How long an ARP entry should stay in cache */
    simtime_t cacheTimeout;

    /** @brief ARP cache entries */
    ArpCache arpCache;
    std::pair<APN, ArpCacheEntry *> thisHost;  ///< Naming information for this host

    /** @brief Where to send arp packets */
    cGate *netwOutGate;

  public:
    RINArp();
    ~RINArp() override;

    /** @brief Attempts to resolve an application name, may send ARP_REQ packet */
    virtual const inet::MACAddress resolveAddress(const APN &apn);

    /** @brief Finds the APN name of an entry by its MAC address */
    virtual APN getAddressFor(const inet::MACAddress &mac) const;

    /** @brief Adds this host's information, necessary for N+1 registration */
    virtual bool addStaticEntry(const inet::MACAddress &mac, const APN &apn);

    /** @brief Removes static entry for connected host */
    virtual void deleteStaticEntry();

  protected:
    /** @brief Processes ARP packets, adds entry if destination is same as apname */
    virtual void processArpPacket(RINArpPacket *arp);

    /** @brief Handles timeout selfmessages */
    virtual void requestTimeout(cMessage *msg);

    /** @brief Removes all entries of ARP cache */
    virtual void flush();

    /** @brief Readies pending entry, sends ARP_REQ, and starts timeout timer */
    virtual void initiateArpResolution(ArpCacheEntry *entry);

    /** @brief Updates an ARP cache entry with an address */
    virtual void updateArpCache(ArpCacheEntry *entry, const inet::MACAddress &mac);

    /** @brief Checks if specified APN is same as in static entry */
    virtual bool addressRecognized(const APN &apn);

    /** @brief Sends packet to network (likely passing it to NIC */
    virtual void sendPacketToNIC(cMessage *msg, const inet::MACAddress &macAddress, int etherType);

    /** @brief Sends an ARP request addressed to specified APN */
    virtual void sendArpRequest(const APN &apn);


    /// cSimpleModule overrides

    /** @brief Initialises parameters and out gate */
    void initialize() override;

    /** @brief Handles selfmessages and messages from ethernet shim module */
    void handleMessage(cMessage *msg) override;
};
