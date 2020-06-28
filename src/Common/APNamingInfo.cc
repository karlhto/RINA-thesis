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

#include "Common/APNamingInfo.h"

APNamingInfo::APNamingInfo(const std::string &name) : apn(name)
{
}

APNamingInfo::APNamingInfo(const APN &apn) : apn(apn)
{
}

APNamingInfo::APNamingInfo(const APN &apn,
                           const std::string &apinstance,
                           const std::string &aename,
                           const std::string &aeinstance)
    : apn(apn), apinstance(apinstance), aename(aename), aeinstance(aeinstance)
{
}

std::string APNamingInfo::str() const
{
    std::ostringstream os;
    os << "AP: " << apn;
    if (!apinstance.empty())
        os << " (" << apinstance << ")";
    if (!aename.empty())
        os << " AE: " << aename;
    if (!aeinstance.empty())
        os << " (" << aeinstance << ")";
    return os.str();
}

// Free function
std::ostream &operator<<(std::ostream &os, const APNamingInfo &apni)
{
    return os << apni.str();
}

Register_Class(APNIPair);

APNIPair::APNIPair() = default;

APNIPair::APNIPair(const APNamingInfo &src, const APNamingInfo &dst) : first(src), second(dst)
{
}

APNIPair::APNIPair(const std::string &src, const std::string &dst)
    : APNIPair(APNamingInfo(src), APNamingInfo(dst))
{
}

std::ostream &operator<<(std::ostream &os, const APNIPair &apnip)
{
    return os << apnip.str();
}

std::string APNIPair::str() const
{
    std::ostringstream os;
    os << "SRC> " << first << "\tDST> " << second;
    return os.str();
}
