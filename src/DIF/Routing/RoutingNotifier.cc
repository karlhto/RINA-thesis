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

#include "DIF/Routing/RoutingNotifier.h"
#include "DIF/RIB/RIBdBase.h"

Define_Module(RoutingNotifier);

const char* MSG_ROUTINGUPDATE   = "RoutingUpdate";

void RoutingNotifier::initialize()
{
    initSignalsAndListeners();
    initPointers();
}

void RoutingNotifier::initPointers()
{
    RIBd = getRINAModule<RIBdBase*>(this, 1, { "ribd" });
}

void RoutingNotifier::initSignalsAndListeners()
{
    cModule* thisIPC = this->getModuleByPath("^.^");

    sigRIBDRoutingUpdateRecv = registerSignal(SIG_RIBD_RoutingUpdateReceived);

    thisIPC->subscribe(SIG_RIBD_RoutingUpdate, this);
}

void RoutingNotifier::handleMessage(cMessage *msg)
{
    EV_ERROR << "This module is not supposed to handle messages" << endl;
    delete msg;
}

void RoutingNotifier::receiveSignal(cComponent *src, simsignal_t id, cObject *obj, cObject *detail)
{
    IntRoutingUpdate * info = dynamic_cast<IntRoutingUpdate *>(obj);
    if (info)
    {
        receiveRoutingUpdateFromRouting(info);
    }
    else
    {
        EV << "ForwardingInfoUpdate received unknown object!" << endl;
    }

    // Unused
    (void)src;
    (void)id;
    (void)detail;
}

void RoutingNotifier::signalizeMessage(CDAPMessage* msg)
{
    RIBd->signalizeSendData(msg);
}

void RoutingNotifier::receiveRoutingUpdateFromRouting(IntRoutingUpdate * info)
{
    EV << getFullPath() << " Forwarding update to send to "
            << info->getDestination();

    /* Emits the CDAP message. */

    CDAP_M_Write * cdapm = new CDAP_M_Write(MSG_ROUTINGUPDATE);
    std::ostringstream os;
    object_t flowobj;

    /* Prepare the object to send. */

    os << "RoutingUpdateTo" << info->getDestination();

    flowobj.objectClass = info->getClassName();
    flowobj.objectName = os.str();
    flowobj.objectVal = info;
    //TODO: Vesely - Assign appropriate values
    flowobj.objectInstance = VAL_DEFINSTANCE;

    cdapm->setObjectItem(flowobj);

    //TODO: Vesely - Work more on InvokeId

    /* This message will be sent to... */
    cdapm->setDstAddr(info->getDestination());

    /* Finally order to send the data... */
    RIBd->signalizeSendData(cdapm);
}

void RoutingNotifier::processMWrite(CDAP_M_Write* msg)
{
    object_t object = msg->getObjectItem();

    if (dynamic_cast<IntRoutingUpdate *>(object.objectVal))
    {
        IntRoutingUpdate* update = check_and_cast<IntRoutingUpdate *>(
                object.objectVal);
        emit(sigRIBDRoutingUpdateRecv, update);
    }
}
