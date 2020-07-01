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

#include "DAF/DA/NeighborTableEntry.h"

NeighborTableEntry::NeighborTableEntry(const APN& napn) :
    apn(napn)
{
}

const APN& NeighborTableEntry::getApn() const {
    return apn;
}

void NeighborTableEntry::setApn(const APN& apn) {
    this->apn = apn;
}

const APNList& NeighborTableEntry::getNeighbors() const {
    return neighbors;
}

bool NeighborTableEntry::operator ==(const NeighborTableEntry& other) const {
    return apn == other.apn && neighbors == other.neighbors;
}

std::string NeighborTableEntry::str() const {
    std::ostringstream os;
    os << "APN: " << apn << ", neighbors: ";
    for (const auto &n : neighbors) {
        os << "\n\t" << n.str();
    }
    return os.str();
}

void NeighborTableEntry::setNeighbors(const APNList& neighbors) {
    this->neighbors = neighbors;
}

void NeighborTableEntry::addNeighbor(const APN& neighbor) {
    neighbors.push_back(neighbor);
}

std::ostream& operator <<(std::ostream& os, const NeighborTableEntry& nte) {
    return os << nte.str();
}

bool NeighborTableEntry::hasNeighbor(const APN& apn) const
{
    for (const auto &neighbor : neighbors) {
        if (neighbor == apn)
            return true;
    }
    return false;
}
