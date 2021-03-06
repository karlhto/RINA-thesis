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

#include "DIF/RA/PDUFG/PDUFGNeighbor.h"

#include "DIF/RMT/RMTPort.h"

const PDUFGNeighbor PDUFGNeighbor::NO_NEIGHBOR = PDUFGNeighbor();

PDUFGNeighbor::PDUFGNeighbor() = default;

PDUFGNeighbor::PDUFGNeighbor(const Address &addr, const QoSCube &qosId, RMTPort *p)
    : dstAddr(addr), qos(qosId), port(p)
{
}

PDUFGNeighbor::~PDUFGNeighbor() = default;

const Address &PDUFGNeighbor::getDestAddr() const
{
    return dstAddr;
}

RMTPort *PDUFGNeighbor::getPort() const
{
    return port;
}

const QoSCube &PDUFGNeighbor::getQoSCube() const
{
    return qos;
}

void PDUFGNeighbor::setDestAddr(const Address &addr)
{
    dstAddr = addr;
}

void PDUFGNeighbor::setPort(RMTPort *p)
{
    port = p;
}

void PDUFGNeighbor::setQosId(const QoSCube &qosId)
{
    qos = qosId;
}

bool operator==(const PDUFGNeighbor &lhs, const PDUFGNeighbor &rhs)
{
    return lhs.getDestAddr() == rhs.getDestAddr() &&
           lhs.getQoSCube().getQosId() == rhs.getQoSCube().getQosId() &&
           lhs.getPort() == rhs.getPort();
}

bool operator!=(const PDUFGNeighbor &lhs, const PDUFGNeighbor &rhs)
{
    return lhs.getDestAddr() != rhs.getDestAddr() ||
           lhs.getQoSCube().getQosId() != rhs.getQoSCube().getQosId() ||
           lhs.getPort() != rhs.getPort();
}
