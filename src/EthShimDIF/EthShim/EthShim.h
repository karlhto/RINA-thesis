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
#include <queue>

#include "Common/APN.h"
#include "EthShimDIF/RINArp/RINArp.h"
#include "inet/linklayer/common/MACAddress.h"
#include "inet/networklayer/common/InterfaceEntry.h"

class RINArpPacket;
class ShimFA;
class SDUData;

/**
 * Implements the main element of the RINA ethernet shim DIF
 */
class EthShim : public cSimpleModule, public cListener
{
  public:
    /// enum used for returning status of a create request
    enum class CreateResult {
        error,
        pending,
        completed
    };

    /// enum used for recording the state of a connection
    enum class ConnectionState : unsigned int {
        none = 0,
        pending,
        allocated,
        _size
    };

    /// Information required for connection with a remote system
    struct ConnectionEntry {
        ConnectionState state = ConnectionState::none;
        cGate *inGate = nullptr;
        cGate *outGate = nullptr;
        cPacketQueue outQueue = cPacketQueue("Queue for outgoing packets");
        cPacketQueue inQueue = cPacketQueue("Queue for incoming packets");
    };

  private:
    /// map containing connection states with remote systems
    std::map<APN, ConnectionEntry> connections;

    /// Pointers to important modules
    cModule *ipcProcess = nullptr;
    RINArp *arp = nullptr;
    ShimFA *shimFA = nullptr;
    inet::InterfaceEntry *ie = nullptr;

    /// Statistics
    long numSentToNetwork = 0;
    long numReceivedFromNetwork = 0;

    /// Provides string names for ConnectionState structs
    static const std::array<std::string, static_cast<unsigned int>(ConnectionState::_size)>
        connInfo;

  public:
    /** @brief Empty constructor for the time being */
    EthShim() = default;

    /** @brief Deletes all dynamically allocated entries in queues */
    ~EthShim() override;

    /**
     * @brief Registers name of the application using this shim IPCP in static ARP entry
     * @param  apn Name of registered application
     */
    void registerApplication(const APN &apn) const;

    /**
     * @brief Finalises a connection entry
     * @param  dstApn Name of application to be reached
     * @param  portId Port ID used as handle for registered application, used in gate names
     * @return true if the bindings were successfully created, false otherwise
     */
    bool addPort(const APN &dstApn, const int &portId);

    // TODO (karlhto): might replace this function with a "hook" for `addPort`
    /**
     * @brief Sends waiting SDUs in queue,
     */
    void sendWaitingIncomingSDUs(const APN &srcApn);

    /**
     * @brief Creates a connection state entry, and starts ARP resolution
     * @param  dstApn Name of application to be reached
     * @return CreateResult::completed if ARP returned an address immediately, ::pending if ARP
     *         resolution was initiated, and ::failed if a connection entry already exists
     */
    CreateResult createEntry(const APN &dstApn);

    /**
     * @brief Give a string representing the state of a ConnectionEntry object
     * @param  connectionEntry
     * @return String representing the state of the connection
     */
    static const std::string &getConnInfoString(const ConnectionEntry &connectionEntry);

  private:
    /// Queue management

    // TODO (karlhto): Rework these functions
    /**
     * @brief Helper function to insert packets into outgoing queue for a connection entry
     * @param dstApn APN of connected application
     * @param sdu    Packet to insert
     */
    void insertOutgoingSDU(const APN &dstApn, SDUData *sdu);

    /**
     * @brief Helper function to insert packets into ingoing queue for a connection entry
     * @param srcApn APN of connected application
     * @param sdu    Packet to insert
     */
    void insertIncomingSDU(const APN &srcApn, SDUData *sdu);


    /// cSimpleModule overrides

    /** @brief Initialises module pointers and subscribes to ARP signals */
    void initialize(int stage) override;
    int numInitStages() const override { return inet::NUM_INIT_STAGES; }

    /** @brief Passes packets to correct helper function based on input */
    void handleMessage(cMessage *msg) override;


    /// Packet handling

    /**
     * @brief Handles SDUs from upper layer, resolving gate to destination address
     * @param  sdu  SDU from upper layer
     * @param  gate Input gate that received sdu
     */
    void handleOutgoingSDU(SDUData *sdu, const cGate *gate);

    /**
     * @brief Handles SDU from network, resolving connection entry from source MAC Address
     * @param  sdu  SDU from ethernet interface
     */
    void handleIncomingSDU(SDUData *sdu);

    /**
     * @brief Handles SDU from network, resolving connection entry from source MAC Address
     * @param  sdu  SDU from ethernet interface
     */
    void handleIncomingArpPacket(RINArpPacket *arpPacket);

    /**
     * @brief Wrapper function for `send(msg, "ifOut")`
     * @param  msg Message to send
     */
    void sendPacketToNIC(cMessage *msg);


    /// cListener overrides, for ARP signals

    /**
     * @brief Retrieves `ArpNotification` objects, passing them to correct helper function
     * @param  source  Source module of signal (unused)
     * @param  id      Id of signal, either completedRINArpResolution or failedRINArpResolution
     * @param  obj     ArpNotification object to be passed on
     * @param  details Unused
     */
    void receiveSignal(cComponent *source, simsignal_t id, cObject *obj, cObject *details) override;

    void arpResolutionCompleted(RINArp::ArpNotification *entry);
    void arpResolutionFailed(RINArp::ArpNotification *entry);
};

std::ostream &operator<<(std::ostream &os, const EthShim::ConnectionEntry &shimEntry);
