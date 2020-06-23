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

#include "EthShimDIF/RINArp/RINArp.h"

#include "EthShimDIF/RINArp/RINArpPacket_m.h"
#include "inet/linklayer/common/Ieee802Ctrl.h"

Define_Module(RINArp);
Register_Abstract_Class(RINArp::ArpNotification);

// Static signal entries
const simsignal_t RINArp::initiatedRINArpResolutionSignal =
    registerSignal("initiatedRINArpResolution");
const simsignal_t RINArp::completedRINArpResolutionSignal =
    registerSignal("completedRINArpResolution");
const simsignal_t RINArp::failedRINArpResolutionSignal = registerSignal("failedRINArpResolution");

RINArp::RINArp()
{
}

RINArp::~RINArp()
{
    for (auto &elem : arpCache) {
        auto *entry = elem.second;
        cancelAndDelete(entry->timer);
        delete entry;
    }

    if (thisHost.second != nullptr)
        delete thisHost.second;
}

void RINArp::initialize()
{
    cSimpleModule::initialize();

    retryTimeout = par("retryTimeout");
    retryCount = par("retryCount");
    cacheTimeout = par("cacheTimeout");

    netwOutGate = gate("netwOut");
}

bool RINArp::addStaticEntry(const inet::MACAddress &mac, const APN &apn)
{
    Enter_Method("addStaticEntry(%s, %s)", mac.str().c_str(), apn.getName().c_str());

    ASSERT(!mac.isUnspecified());
    ASSERT(!apn.isUnspecified());

    // Is already in use
    if (!thisHost.first.isUnspecified())
        return false;

    ArpCacheEntry *entry = new ArpCacheEntry();
    entry->macAddress = mac;
    entry->timer = nullptr;
    entry->lastUpdate = simTime();
    thisHost = std::make_pair(apn, entry);
    return true;
}

void RINArp::deleteStaticEntry()
{
    Enter_Method("deleteStaticEntry()");
    if (!thisHost.first.isUnspecified())
        thisHost.first.setName("");

    if (thisHost.second != nullptr) {
        auto *entry = thisHost.second;
        cancelAndDelete(entry->timer);
        entry->timer = nullptr;
        delete entry;
    }
}

bool RINArp::addressRecognized(const APN &destApn)
{
    if (destApn == thisHost.first)
        return true;
    else
        return false;
}

void RINArp::flush()
{
    while (!arpCache.empty()) {
        auto i = arpCache.begin();
        auto *entry = i->second;
        cancelAndDelete(entry->timer);
        entry->timer = nullptr;
        delete entry;
        arpCache.erase(i);
    }
}

void RINArp::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
        requestTimeout(msg);
    else {
        auto *arp = check_and_cast<RINArpPacket *>(msg);
        processArpPacket(arp);
    }
}

void RINArp::updateArpCache(ArpCacheEntry *entry, const inet::MACAddress &macAddress)
{
    if (entry->pending) {
        entry->pending = false;
        cancelAndDelete(entry->timer);
        entry->timer = nullptr;
        entry->numRetries = 0;
    }
    entry->macAddress = macAddress;
    entry->lastUpdate = simTime();
    ArpNotification signal(entry->myIter->first, macAddress);
    emit(completedRINArpResolutionSignal, &signal);
}

const inet::MACAddress RINArp::resolveAddress(const APN &apn)
{
    Enter_Method("resolveAddress(%s)", apn.getName().c_str());

    EV << "Asked to resolve destination address " << apn << endl;

    ArpCache::const_iterator it = arpCache.find(apn);
    if (it == arpCache.end()) {
        // No ARP cache entry found, need to send ARP request
        auto *entry = new ArpCacheEntry();
        auto where = arpCache.insert(arpCache.begin(), std::make_pair(apn, entry));
        entry->myIter = where;
        initiateArpResolution(entry);
        return inet::MACAddress::UNSPECIFIED_ADDRESS;
    } else if (it->second->pending) {
        return inet::MACAddress::UNSPECIFIED_ADDRESS;
    } else if (it->second->lastUpdate + cacheTimeout >= simTime()) {
        return it->second->macAddress;
    }

    // If entry exists but has been timed out
    auto *entry = it->second;
    initiateArpResolution(entry);
    return inet::MACAddress::UNSPECIFIED_ADDRESS;
}

APN RINArp::getAddressFor(const inet::MACAddress &mac) const
{
    Enter_Method_Silent();

    if (mac.isUnspecified())
        return APN::UNSPECIFIED_APN;

    simtime_t now = simTime();
    for (const auto &elem : arpCache)
        if (elem.second->macAddress == mac && elem.second->lastUpdate + cacheTimeout >= now)
            return elem.first;

    return APN::UNSPECIFIED_APN;
}

void RINArp::initiateArpResolution(ArpCacheEntry *entry)
{
    const APN &apn = entry->myIter->first;
    entry->pending = true;
    entry->numRetries = 0;
    entry->lastUpdate = SIMTIME_ZERO;
    entry->macAddress = inet::MACAddress::UNSPECIFIED_ADDRESS;
    sendArpRequest(apn);

    cMessage *msg = entry->timer = new cMessage("ARP timeout");
    msg->setContextPointer(entry);
    scheduleAt(simTime() + retryTimeout, msg);

    ArpNotification signal(apn, inet::MACAddress::UNSPECIFIED_ADDRESS);
    emit(initiatedRINArpResolutionSignal, &signal);
}

void RINArp::sendArpRequest(const APN &dstApn)
{
    const APN &srcApn = thisHost.first;  // Registered application
    const inet::MACAddress &srcMac = thisHost.second->macAddress;

    ASSERT(!srcMac.isUnspecified());
    ASSERT(!srcApn.isUnspecified());

    int apnLen = std::max(srcApn.length(), dstApn.length());
    RINArpPacket *arp = new RINArpPacket("arpREQ");
    arp->setByteLength(ARP_BASE_LEN + apnLen * 2);
    arp->setApnLength(apnLen);
    arp->setOpcode(ARP_REQUEST);
    arp->setSrcMacAddress(srcMac);
    arp->setSrcApName(srcApn);
    arp->setDstApName(dstApn);
    sendPacketToNIC(arp, inet::MACAddress::BROADCAST_ADDRESS, inet::ETHERTYPE_ARP);
}

void RINArp::processArpPacket(RINArpPacket *arp)
{
    EV_INFO << "Received " << arp << " from ethernet shim." << endl;
    const inet::MACAddress srcMac = arp->getSrcMacAddress();
    const APN srcApn = arp->getSrcApName();
    const APN dstApn = arp->getDstApName();

    ASSERT(!srcMac.isUnspecified());
    ASSERT(!srcApn.isUnspecified());
    ASSERT(!dstApn.isUnspecified());

    bool mergeFlag = false;
    auto it = arpCache.find(srcApn);
    if (it != arpCache.end()) {
        ArpCacheEntry *entry = it->second;
        updateArpCache(entry, srcMac);
        mergeFlag = true;
    }

    if (addressRecognized(dstApn)) {
        if (!mergeFlag) {
            ArpCacheEntry *entry;
            if (it != arpCache.end()) {
                entry = it->second;
            } else {
                entry = new ArpCacheEntry();
                auto where = arpCache.insert(arpCache.begin(), std::make_pair(srcApn, entry));
                entry->myIter = where;
                entry->pending = false;
                entry->timer = nullptr;
                entry->numRetries = 0;
            }

            updateArpCache(entry, srcMac);
        }

        switch (arp->getOpcode()) {
        case ARP_REQUEST: {
            // Protocol address length will remain the same. We need to
            // swap the addresses, however.
            arp->setName("arpREPLY");
            arp->setDstMacAddress(srcMac);
            arp->setDstApName(srcApn);
            arp->setSrcMacAddress(thisHost.second->macAddress);
            arp->setSrcApName(dstApn);
            arp->setOpcode(ARP_REPLY);
            delete arp->removeControlInfo();
            sendPacketToNIC(arp, srcMac, inet::ETHERTYPE_ARP);

            break;
        }
        case ARP_REPLY: {
            delete arp;
            break;
        }
        case ARP_RARP_REQUEST:
            throw cRuntimeError("RARP request received but not supported");
        case ARP_RARP_REPLY:
            throw cRuntimeError("RARP reply received but not supported");
        default:
            throw cRuntimeError(
                "Unsuported opcode %d from received Arp"
                " packet",
                arp->getOpcode());
        }
    } else {
        EV_INFO << "Address " << dstApn << " not recognized, "
                << "dropping packet." << endl;
        delete arp;
    }
}

void RINArp::sendPacketToNIC(cMessage *msg, const inet::MACAddress &macAddress, int etherType)
{
    inet::Ieee802Ctrl *controlInfo = new inet::Ieee802Ctrl();
    controlInfo->setDest(macAddress);
    controlInfo->setEtherType(etherType);
    msg->setControlInfo(controlInfo);

    EV_INFO << "Sending " << msg << " to ethernet shim";
    send(msg, netwOutGate);
}

void RINArp::requestTimeout(cMessage *selfmsg)
{
    ArpCacheEntry *entry = (ArpCacheEntry *)selfmsg->getContextPointer();
    entry->numRetries++;
    if (entry->numRetries < retryCount) {
        const APN &dstApn = entry->myIter->first;
        EV_INFO << "ARP request for " << dstApn << " timed out, resending." << endl;
        sendArpRequest(dstApn);
        scheduleAt(simTime() + retryTimeout, selfmsg);
        return;
    }

    // If max retryCount hit
    delete selfmsg;

    EV << "ARP timed out with max retry count " << retryCount << " for destination address "
       << entry->myIter->first << endl;
    ArpNotification signal(entry->myIter->first, inet::MACAddress::UNSPECIFIED_ADDRESS);
    emit(failedRINArpResolutionSignal, &signal);
    arpCache.erase(entry->myIter);
    delete entry;
}
