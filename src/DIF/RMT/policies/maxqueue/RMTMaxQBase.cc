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

#include <RMTMaxQBase.h>

Define_Module(RMTMaxQBase);


RMTMaxQBase::RMTMaxQBase()
{
}

RMTMaxQBase::~RMTMaxQBase()
{
}

void RMTMaxQBase::initialize()
{
    qMonPolicy = check_and_cast<RMTQMonitorBase*>
        (getModuleByPath("^.queueMonitorPolicy"));

    // display active policy name
    cDisplayString& disp = getDisplayString();
    disp.setTagArg("t", 1, "t");
    disp.setTagArg("t", 0, getClassName());

    onPolicyInit();
}

void RMTMaxQBase::onPolicyInit()
{
}

void RMTMaxQBase::handleMessage(cMessage *msg)
{

}

bool RMTMaxQBase::run(RMTQueue* queue)
{
    return false;
}
