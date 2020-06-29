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

// Author: Kewin Rausch (kewin.rausch@create-net.org)

#ifndef __RINA_PDUFGNEIGHBOR_H
#define __RINA_PDUFGNEIGHBOR_H

#include <omnetpp.h>

#include "Common/Address.h"
#include "Common/QoSCube.h"

class RMTPort;

// How is considered a neighbor for the PDUFTG module.
//
class PDUFGNeighbor
{
  private:
    Address dstAddr;
    QoSCube qos;
    RMTPort *port;

  public:
    PDUFGNeighbor();
    PDUFGNeighbor(const Address &dst, const QoSCube &qos, RMTPort *port);
    ~PDUFGNeighbor();

    [[nodiscard]] const Address &getDestAddr() const;
    [[nodiscard]] RMTPort *getPort() const;
    [[nodiscard]] const QoSCube &getQoSCube() const;

    void setDestAddr(const Address &dstAddr);
    void setPort(RMTPort *p);
    void setQosId(const QoSCube &qosId);

    static const PDUFGNeighbor NO_NEIGHBOR;
};

/**
 * @brief Equal operator overloading
 * @return True if PDUFGNeighbor entries are the same
 */
bool operator==(const PDUFGNeighbor &lhs, const PDUFGNeighbor &rhs);

/**
 * @brief Not equal operator overloading
 * @return True if the entries of PDUFGNeighbor are not the same
 */
bool operator!=(const PDUFGNeighbor &lhs, const PDUFGNeighbor &rhs);

#endif
