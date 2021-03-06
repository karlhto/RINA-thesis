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

#include "Common/Address.h"

const Address Address::UNSPECIFIED_ADDRESS = Address();

Address::Address(std::string composite)
{
    std::string delimiter = "_";

    size_t pos = 0;
    std::vector<std::string> tokens;
    while ((pos = composite.find(delimiter)) != std::string::npos) {
        tokens.push_back(composite.substr(0, pos));
        composite.erase(0, pos + delimiter.length());
    }
    tokens.push_back(composite);

    if (tokens.size() == 2) {
        ipcAddress = APN(tokens.front());
        difName = DAP(tokens.back());
        std::ostringstream os;
        os << ipcAddress << "_" << difName;
        apname = APN(os.str().c_str());
    }

    if (tokens.size() == 1) {
        apname = APN(composite);
    }
}

Address::Address(const APN &apname) : Address(apname.getName())
{
}

Address::Address(const char *ipcaddr, const char *difnam)
    : ipcAddress(ipcaddr), difName(difnam)
{
    updateApname();
}

const DAP &Address::getDifName() const
{
    return difName;
}

void Address::setDifName(const DAP &difName)
{
    this->difName = difName;
    updateApname();
}

const APN &Address::getIpcAddress() const
{
    return ipcAddress;
}

void Address::updateApname() {
    std::ostringstream os;
    os << ipcAddress << "_" << difName;
    apname = APN(os.str());
}

std::string Address::str() const
{
    std::ostringstream os;
    if (!ipcAddress.isUnspecified() && !difName.isUnspecified())
        os << ipcAddress << "(" << difName << ")";
    return os.str();
}

bool Address::isUnspecified() const
{
    return ipcAddress.isUnspecified() && difName.isUnspecified();
}

const APN &Address::getApn() const
{
    return apname;
}

void Address::setIpcAddress(const APN &ipcAddress)
{
    this->ipcAddress = ipcAddress;
    updateApname();
}

std::ostream &operator<<(std::ostream &os, const Address &addr)
{
    return os << addr.str();
}

std::ostream &operator<<(std::ostream &os, const Addresses &dims)
{
    for (const auto &dim : dims)
        os << dim.str() << endl;
    return os;
}

bool operator==(const Address &lhs, const Address &rhs)
{
    return lhs.getApn() == rhs.getApn();
}

bool operator!=(const Address &lhs, const Address &rhs)
{
    return lhs.getApn() != rhs.getApn();
}

bool operator<(const Address &lhs, const Address &rhs)
{
    return lhs.getApn() < rhs.getApn();
}

bool operator<=(const Address &lhs, const Address &rhs)
{
    return lhs.getApn() <= rhs.getApn();
}

bool operator>(const Address &lhs, const Address &rhs)
{
    return lhs.getApn() > rhs.getApn();
}

bool operator>=(const Address &lhs, const Address &rhs)
{
    return lhs.getApn() >= rhs.getApn();
}
