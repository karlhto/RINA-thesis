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

#include "DIF/Routing/IntRouting.h"

#include "Common/ExternConsts.h"
#include "Common/Utils.h"

void IntRouting::finish()
{
    cModule *catcher = this->getParentModule();
    catcher->unsubscribe(SIG_RIBD_RoutingUpdateReceived, this);
}

void IntRouting::initialize()
{
    // IPCProcess module.
    cModule *ipcModule = getParentModule();

    myAddress = Address(ipcModule->par("ipcAddress").stringValue(),
                        ipcModule->par("difName").stringValue());

    getRINAModule<cObject *>(this, 1, {MOD_RESALLOC, MOD_PDUFWDGEN});

    // Set FWDGenerator
    fwdg = getRINAModule<IntPDUFG *>(this, 1, {MOD_RESALLOC, MOD_PDUFWDGEN});

    // Display active policy name.
    setPolicyDisplayString(this);

    cModule *catcher = this->getParentModule();

    // Signal emitters; there will be part of our outputs.
    sigRoutingUpdate = registerSignal(SIG_RIBD_RoutingUpdate);

    // Signal receivers; there will be part of our inputs.
    catcher->subscribe(SIG_RIBD_RoutingUpdateReceived, this);

    onPolicyInit();
}

void IntRouting::sendUpdate(IntRoutingUpdate *update)
{
    update->setSource(myAddress);
    emit(sigRoutingUpdate, update);
}

void IntRouting::receiveUpdate(IntRoutingUpdate *update)
{
    if (processUpdate(update))
        fwdg->routingUpdated();
}

void IntRouting::receiveSignal(cComponent *src, simsignal_t id, cObject *obj, cObject *details)
{
    EV << "RoutingUpdate initiated by " << src->getFullPath() << " and processed by "
       << getFullPath() << endl;

    IntRoutingUpdate *update = dynamic_cast<IntRoutingUpdate *>(obj);

    if (update)
        receiveUpdate(update);
    else
        EV << "RoutingUpdate received unknown object!" << endl;

    delete update;
}
