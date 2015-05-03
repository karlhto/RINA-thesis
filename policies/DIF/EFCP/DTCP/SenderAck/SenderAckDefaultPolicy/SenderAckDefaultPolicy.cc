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
/**
 * @file SenderAckPolicyDefault.cc
 * @author Marcel Marek (imarek@fit.vutbr.cz)
 * @date May 3, 2015
 * @brief This is an example policy class implementing default SenderAck behavior
 * @detail
 */

#include <SenderAckDefaultPolicy/SenderAckDefaultPolicy.h>

Register_Class(SenderAckDefaultPolicy);

SenderAckDefaultPolicy::SenderAckDefaultPolicy()
{


}

SenderAckDefaultPolicy::~SenderAckDefaultPolicy()
{

}

bool SenderAckDefaultPolicy::run(DTPState* dtpState, DTCPState* dtcpState)
{
  Enter_Method("SenderAckPolicyDefault");

  defaultAction(dtpState, dtcpState);

  return false;
}
