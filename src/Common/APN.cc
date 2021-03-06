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

#include "Common/APN.h"

const APN APN::UNSPECIFIED_APN = APN();

APN::APN(const std::string &name) : name(name)
{
}

const std::string &APN::getName() const
{
    return name;
}

void APN::setName(const std::string &name)
{
    this->name = name;
}

std::string APN::str() const
{
    return name;
}

const char *APN::c_str() const
{
    return name.c_str();
}

std::string::size_type APN::length() const
{
    return name.length();
}

bool APN::isUnspecified() const
{
    return name.empty();
}

std::ostream &operator<<(std::ostream &os, const APN &apn)
{
    return os << apn.str();
}

std::ostream &operator<<(std::ostream &os, const APNList &apns)
{
    for (const auto &apn : apns)
        os << apn << " ";
    return os;
}
