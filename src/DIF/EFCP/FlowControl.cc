//
// Copyright © 2014 PRISTINE Consortium (http://ict-pristine.eu)
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

/*
 * @file FlowControl.cc
 * @author Marcel Marek (imarek@fit.vutbr.cz)
 * @date May 3, 2014
 * @brief
 * @detail
 */

#include <FlowControl.h>

FlowControl::FlowControl()
{
  // TODO Auto-generated constructor stub

}

unsigned int FlowControl::getSendRightWindowEdge() const
{
  return sendRightWindowEdge;
}

void FlowControl::setSendRightWindowEdge(unsigned int sendRightWindowEdge)
{
  this->sendRightWindowEdge = sendRightWindowEdge;
}

FlowControl::~FlowControl()
{
  // TODO Auto-generated destructor stub
}
