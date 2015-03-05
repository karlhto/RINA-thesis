//
// Copyright © 2014 - 2015 PRISTINE Consortium (http://ict-pristine.eu)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __PrefixMatch_H_
#define __PrefixMatch_H_

#include <omnetpp.h>
#include <vector>
#include <string>

#include "AddressComparatorBase.h"

class PrefixMatch : public AddressComparatorBase
{
    public:
        virtual bool matchesThisIPC(const Address& addr);
    protected:
        std::vector<std::string> thisIPCAddrParsed;
        std::string any;
        char delimiter;

        virtual void onPolicyInit();

        std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
        std::vector<std::string> split(const std::string &s, char delim);
};

#endif
