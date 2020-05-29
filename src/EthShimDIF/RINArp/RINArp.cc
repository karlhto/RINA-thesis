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
#include "inet/linklayer/ethernet/EtherMAC.h"
#include "inet/linklayer/common/Ieee802Ctrl.h"

Define_Module(RINArp);

RINArp::RINArp() : srcMac() {}

RINArp::~RINArp() {
    for (auto &elem : arpCache)
        delete elem.second;
}

void RINArp::initialize(int stage) {
    cSimpleModule::initialize(stage);

    // Stages are needed to make sure MAC address is available
    if (stage == inet::INITSTAGE_LOCAL) {
        retryTimeout = par("retryTimeout");
        retryCount = par("retryCount");
        cacheTimeout = par("cacheTimeout");

        netwOutGate = gate("netwOut");
    }
    else if (stage == inet::INITSTAGE_LINK_LAYER) {
        ipcProcess = getParentModule();
        interface = dynamic_cast<cModule *>(ipcProcess->getModuleByPath(".eth.mac"));
        // Address should be available now
        srcMac.setAddress(interface->par("address"));
    }
}

bool RINArp::registerAp(const APN &addr) {
    Enter_Method("registerAp(%s)", addr.getName().c_str());

    // Is already in use
    if (!apName.isUnspecified()) {
        return false;
    }

    apName = addr;
    return true;
}

bool RINArp::addressRecognized(const APN &destAddr) {
    if (destAddr == apName) {
        return true;
    }
    else {
        return false;
    }
}

void RINArp::flush() {
    while (!arpCache.empty()) {
        auto i = arpCache.begin();
        auto *entry = i->second;
        cancelAndDelete(entry->timer);
        entry->timer = nullptr;
        delete entry;
        arpCache.erase(i);
    }
}

void RINArp::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        requestTimeout(msg);
    }
    else {
        auto *arp = check_and_cast<RINArpPacket *>(msg);
        processArpPacket(arp);
    }
}

void RINArp::updateArpCache(ArpCacheEntry *entry,
                            const inet::MACAddress& macAddress) {
    if (entry->pending) {
        entry->pending = false;
        delete cancelEvent(entry->timer);
        entry->timer = nullptr;
        entry->numRetries = 0;
    }
    entry->dstMac = macAddress;
    entry->lastUpdate = simTime();
}

const inet::MACAddress RINArp::resolveAddress(const APN &addr) {
    Enter_Method("resolveAddress(%s)", addr.getName().c_str());

    EV << "Asked to resolve destination address " << addr << endl;

    ArpCache::const_iterator it = arpCache.find(addr);
    if (it == arpCache.end()) {
        // No ARP cache entry found, need to send ARP request
        auto *entry = new ArpCacheEntry();
        auto where = arpCache.insert(arpCache.begin(),
                                     std::make_pair(addr, entry));
        entry->myIter = where;
        initiateArpResolution(entry);
        return inet::MACAddress::UNSPECIFIED_ADDRESS;
    }
    else if (it->second->pending) {
        return inet::MACAddress::UNSPECIFIED_ADDRESS;
    }
    else if (it->second->lastUpdate + cacheTimeout >= simTime()) {
        return it->second->dstMac;
    }

    auto *entry = it->second;
    initiateArpResolution(entry);
    return inet::MACAddress::UNSPECIFIED_ADDRESS;
}

void RINArp::initiateArpResolution(ArpCacheEntry *entry) {
    const APN &addr = entry->myIter->first;
    entry->pending = true;
    entry->numRetries = 0;
    entry->lastUpdate = SIMTIME_ZERO;
    entry->dstMac = inet::MACAddress::UNSPECIFIED_ADDRESS;
    sendArpRequest(addr);

    cMessage *msg = entry->timer = new cMessage("ARP timeout");
    msg->setContextPointer(entry);
    scheduleAt(simTime() + retryTimeout, msg);
}

void RINArp::sendArpRequest(const APN &dstAddr) {
    const inet::MACAddress &srcMac = this->srcMac;
    const APN &srcAddr = this->apName; // Registered application

    ASSERT(!srcMac.isUnspecified());
    ASSERT(!srcAddr.isUnspecified());

    int apnLen = std::max(srcAddr.length(), dstAddr.length());
    RINArpPacket *arp = new RINArpPacket("arpREQ");
    arp->setByteLength(ARP_BASE_LEN + apnLen * 2);
    arp->setApnLength(apnLen);
    arp->setOpcode(ARP_REQUEST);
    arp->setSrcMacAddress(srcMac);
    arp->setSrcApName(srcAddr);
    arp->setDstApName(dstAddr);
    sendPacketToNIC(arp, inet::MACAddress::BROADCAST_ADDRESS,
                    inet::ETHERTYPE_ARP);
}

void RINArp::processArpPacket(RINArpPacket *arp) {
    EV_INFO << "Received " << arp << " from ethernet shim." << endl;
    const inet::MACAddress srcMacAddr = arp->getSrcMacAddress();
    const APN srcAddr = arp->getSrcApName();
    const APN dstAddr = arp->getDstApName();

    ASSERT(!srcMacAddr.isUnspecified());
    ASSERT(!srcAddr.isUnspecified());
    ASSERT(!dstAddr.isUnspecified());

    bool mergeFlag = false;
    auto it = arpCache.find(srcAddr);
    if (it != arpCache.end()) {
        ArpCacheEntry *entry = it->second;
        updateArpCache(entry, srcMacAddr);
        mergeFlag = true;
    }

    if (addressRecognized(dstAddr)) {
        if (!mergeFlag) {
            ArpCacheEntry *entry;
            if (it != arpCache.end()) {
                entry = it->second;
            }
            else {
                entry = new ArpCacheEntry();
                auto where = arpCache.insert(arpCache.begin(),
                                             std::make_pair(srcAddr, entry));
                entry->myIter = where;
                entry->pending = false;
                entry->timer = nullptr;
                entry->numRetries = 0;
            }

            updateArpCache(entry, srcMacAddr);
        }

        switch (arp->getOpcode()) {
            case ARP_REQUEST: {
                // Protocol address length will remain the same. We need to
                // swap the addresses, however.
                arp->setName("arpREPLY");
                arp->setDstMacAddress(srcMacAddr);
                arp->setDstApName(srcAddr);
                arp->setSrcMacAddress(srcMac);
                arp->setSrcApName(dstAddr);
                arp->setOpcode(ARP_REPLY);
                delete arp->removeControlInfo();
                sendPacketToNIC(arp, srcMacAddr, inet::ETHERTYPE_ARP);

                break;
            }
            case ARP_REPLY: {
                // We also need to notify here
                delete arp;
                break;
            }
            case ARP_RARP_REQUEST:
                throw cRuntimeError("RARP request received but not supported");
            case ARP_RARP_REPLY:
                throw cRuntimeError("RARP reply received but not supported");
            default:
                throw cRuntimeError("Unsuported opcode %d from received Arp"
                                    " packet", arp->getOpcode());
        }
    }
    else {
        EV_INFO << "Address " << dstAddr << " not recognized, "
            << "dropping packet." << endl;
        delete arp;
    }
}

void RINArp::sendPacketToNIC(cMessage *msg,
                             const inet::MACAddress &macAddress,
                             int etherType) {
    inet::Ieee802Ctrl *controlInfo = new inet::Ieee802Ctrl();
    controlInfo->setDest(macAddress);
    controlInfo->setEtherType(etherType);
    msg->setControlInfo(controlInfo);

    EV_INFO << "Sending " << msg << " to ethernet shim";
    send(msg, netwOutGate);
}

void RINArp::requestTimeout(cMessage *selfmsg) {
    ArpCacheEntry *entry = (ArpCacheEntry *)selfmsg->getContextPointer();
    entry->numRetries++;
    if (entry->numRetries < retryCount) {
        const APN &dstAddr = entry->myIter->first;
        EV_INFO << "ARP request for " << dstAddr << " timed out, resending." << endl;
        sendArpRequest(dstAddr);
        scheduleAt(simTime() + retryTimeout, selfmsg);
        return;
    }

    delete selfmsg;

    EV << "ARP timed out with max retry count " << retryCount
        << " for destination address " << entry->myIter->first << endl;
    arpCache.erase(entry->myIter);
    delete entry;
    // need to tell either ethshim or shimfa about this, probably with
    // an Enter_Method call
}
