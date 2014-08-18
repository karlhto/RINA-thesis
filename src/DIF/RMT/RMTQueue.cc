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

#include "RMTQueue.h"


RMTQueue::RMTQueue()
{
    EV << "creating a RMTqueue" << endl;
    this->isActive = true;
}

RMTQueue::RMTQueue(int MaxQLength, int threshQLength, int qLength)
{
    EV << "creating a specified RMTqueue" << endl;
    this->isActive = true;
}


RMTQueue::~RMTQueue()
{
    EV << "destroying a RMTqueue" << endl;
}


void RMTQueue::initialize()
{

}

void RMTQueue::handleMessage(cMessage *msg)
{

}

void RMTQueue::insertPDU(DataTransferPDU pdu)
{
    queue.push_back(pdu);
}

int RMTQueue::getMaxLength()
{
    return maxQLength;
}

int RMTQueue::getThreshLength()
{
    return thresholdQLength;
}

int RMTQueue::getLength()
{
    return qLength;
}

void RMTQueue::setMaxLength(int val)
{
    this->maxQLength = val;
}

void RMTQueue::setThreshLength(int val)
{
    this->thresholdQLength = val;
}

void RMTQueue::setLength(int val)
{
    this->qLength = val;
}

bool RMTQueue::qState()
{
    return isActive;
}
void RMTQueue::suspendQ()
{
    this->isActive = false;
}

void RMTQueue::resumeQ()
{
    this->isActive = true;
}
