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

#pragma once

#include "DIF/RMT/Monitor/ModularMonitor/Int_MM_Out_Module.h"

#include <string>
#include <map>
#include <vector>
#include <list>

namespace MM_maxPST_Out {

using namespace std;

class MM_maxPST_Out : public Int_MM_Out_Module
{
    public:
    ~MM_maxPST_Out();
        void finish();

        void pduInsertered(RMTQueue *, RMTPort *);
        void pduDropped(RMTQueue *, const cPacket *, RMTPort *);
        void pduReleased(RMTQueue *, RMTPort *);
        void queueCreated(RMTQueue *, RMTPort *);

        RMTQueue * getnextQueue(RMTPort *);
        simtime_t  getnextTime(RMTPort *);

    protected:
        map<string , int> qName2Threshold, qName2PrePrio, qName2PostPrio;
        map<RMTQueue * , int> qThreshold, qPrePrio, qPostPrio;

        map<RMTPort* , set<RMTQueue*> > portQueues;
        map<RMTPort* , long > portTime;
        map<RMTQueue* , list<long> > queueInTime;

        int maxTH, margin;

        void initialize();
};

}
