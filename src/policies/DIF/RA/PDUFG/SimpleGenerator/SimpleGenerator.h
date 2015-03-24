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

#ifndef SimpleGenerator_H_
#define SimpleGenerator_H_

#include <IntPDUFG.h>
#include <SimpleTable/SimpleTable.h>
#include <SimpleRouting/IntSimpleRouting.h>

#include <map>
#include <set>

namespace SimpleGenerator {

typedef std::set<RMTPort*> PortsSet;
typedef std::map<unsigned short, PortsSet> Nentries;
typedef std::map<std::string, Nentries> NTable;

typedef PortsSet::iterator PortsSetIt;
typedef Nentries::iterator NentriesIt;
typedef NTable::iterator NTableIt;


class SimpleGenerator: public IntPDUFG {
public:
    // A new flow has been inserted/or removed
    virtual void insertedFlow(const Address &addr, const unsigned short &qos, RMTPort * port);
    virtual void removedFlow(const Address &addr, const unsigned short &qos, RMTPort * port);

    //Routing has processes a routing update
    virtual void routingUpdated();

protected:
    // Called after initialize
    virtual void onIni();

private:
    DA * difA;
    SimpleTable::SimpleTable * fwd;
    IntSimpleRouting * rt;

    NTable neighbours;
};

}

#endif /* STATICGENERATOR_H_ */
