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

/**
 * @file Enrollment.cc
 * @author Kamil Jerabek (xjerab18@stud.fit.vutbr.cz)
 * @date Apr 1, 2015
 * @brief Enrollment and CACE
 * @detail
 */


#include "DIF/Enrollment/Enrollment.h"

#include "Common/Address.h"
#include "Common/ExternConsts.h"
#include "Common/Flow.h"
#include "Common/RINASignals.h"
#include "Common/Utils.h"
#include "DAF/CDAP/CDAPMessage_m.h"
#include "DAF/IRM/ConnectionTable.h"
#include "DIF/Enrollment/EnrollmentListeners.h"
#include "DIF/Enrollment/EnrollmentObj.h"
#include "DIF/Enrollment/EnrollmentStateTable.h"
#include "DIF/Enrollment/OperationObj.h"
#include "DIF/FA/FABase.h"
#include "DIF/RIB/RIBd.h"

Define_Module(Enrollment);

const char* PAR_AUTH_TYPE       = "authType";
const char* PAR_AUTH_NAME       = "authName";
const char* PAR_AUTH_OTHER      = "authOther";
const char* PAR_AUTH_PASS       = "authPassword";
const char* PAR_CON_RETRIES     = "maxConRetries";
const char* PAR_ISSELFENROL     = "isSelfEnrolled";

const char* MSG_CONREQ                = "Connect/Auth";
const char* MSG_CONREQRETRY           = "ConnectRetry/Auth";
const char* MSG_CONRESPOS             = "Connect+/Auth";
const char* MSG_CONRESNEG             = "Connect-/Auth";
const char* MSG_ENRLCON               = "Enrol-Connect";
const char* MSG_ENRLREL               = "Enrol-Release";

const char* ELEM_PREENROL     = "Preenrollment";
const char* ELEM_SIMTIME      = "SimTime";
const char* ELEM_CONNECT      = "Connect";
const char* ELEM_RELEASE      = "Release";
const char* ATTR_TIME         = "t";

Enrollment::~Enrollment() {
    for (auto &elem : preConnects)
        if (elem.second != nullptr)
            delete elem.second;

    for (auto &elem : preReleases)
        if (elem.second != nullptr)
            delete elem.second;

    if (lisEnrollmentConReq != nullptr) {
        delete lisEnrollmentStartEnrollReq;
        delete lisEnrollmentStartEnrollRes;
        delete lisEnrollmentStopEnrollReq;
        delete lisEnrollmentStopEnrollRes;
        delete lisEnrollmentStartOperationReq;
        delete lisEnrollmentStartOperationRes;
        delete lisEnrollmentConResPosi;
        delete lisEnrollmentConResNega;
        delete lisEnrollmentConReq;
    }
}

void Enrollment::initialize()
{
    //Parse XML config
    parseConfig(par(PAR_CONFIGDATA).xmlValue());

    initSignalsAndListeners();
    initPointers();

    //Perform self-enrollment
    bool isSelfEnrol = par(PAR_ISSELFENROL).boolValue();
    if (isSelfEnrol) {
        stateTable->insert(EnrollmentStateTableEntry(
                APNamingInfo(flowAlloc->getMyAddress().getApn()),
                APNamingInfo(flowAlloc->getMyAddress().getApn()),
                EnrollmentStateTableEntry::CON_ESTABLISHED,
                EnrollmentStateTableEntry::ENROLL_ENROLLED));
        updateEnrollmentDisplay(ENICON_ENROLLED);
    }
    else {
        //TODO: Work more on checking of N-1 flow existence
        if (stateTable->isEnrolled(flowAlloc->getMyAddress().getApn()))
            { updateEnrollmentDisplay(ENICON_FLOWMIS); }
        else
            { updateEnrollmentDisplay(ENICON_NOTENROLLED); }
    }

    authType = par(PAR_AUTH_TYPE);
    authName = this->par(PAR_AUTH_NAME).stringValue();
    authPassword = this->par(PAR_AUTH_PASS).stringValue();
    authOther = this->par(PAR_AUTH_OTHER).stringValue();

    maxConRetries = this->par(PAR_CON_RETRIES);


    WATCH_MAP(preConnects);
    WATCH_MAP(preReleases);
}

void Enrollment::initPointers(){
    stateTable = getRINAModule<EnrollmentStateTable*>(this, 1, {MOD_ENROLLMENTTABLE});
    ribDaemon = getRINAModule<RIBd*>(this, 2, {MOD_RIBDAEMON, MOD_RIBD});
    flowAlloc = getRINAModule<FABase*>(this, 2, {MOD_FLOWALLOC, MOD_FA});
}

void Enrollment::initSignalsAndListeners() {
    cModule* catcher1 = this->getModuleByPath("^.^");

    sigEnrollmentCACESendData   = registerSignal(SIG_ENROLLMENT_CACEDataSend);
    sigEnrollmentSendData       = registerSignal(SIG_ENROLLMENT_DataSend);
    sigEnrollmentStartEnrollReq = registerSignal(SIG_ENROLLMENT_StartEnrollmentRequest);
    sigEnrollmentStartEnrollRes = registerSignal(SIG_ENROLLMENT_StartEnrollmentResponse);
    sigEnrollmentStopEnrollReq  = registerSignal(SIG_ENROLLMENT_StopEnrollmentRequest);
    sigEnrollmentStopEnrollRes  = registerSignal(SIG_ENROLLMENT_StopEnrollmentResponse);
    sigEnrollmentStartOperReq   = registerSignal(SIG_ENROLLMENT_StartOperationRequest);
    sigEnrollmentStartOperRes   = registerSignal(SIG_ENROLLMENT_StartOperationResponse);

    //lisEnrollmentGetFlowFromFaiCreResPosi = new LisEnrollmentGetFlowFromFaiCreResPosi(this);
    //catcher1->subscribe(SIG_FAI_CreateFlowResponsePositive, lisEnrollmentGetFlowFromFaiCreResPosi);

    lisEnrollmentStartEnrollReq = new LisEnrollmentStartEnrollReq(this);
    catcher1->subscribe(SIG_RIBD_StartEnrollmentRequest, lisEnrollmentStartEnrollReq);

    lisEnrollmentStartEnrollRes = new LisEnrollmentStartEnrollRes(this);
    catcher1->subscribe(SIG_RIBD_StartEnrollmentResponse, lisEnrollmentStartEnrollRes);

    lisEnrollmentStopEnrollReq = new LisEnrollmentStopEnrollReq(this);
    catcher1->subscribe(SIG_RIBD_StopEnrollmentRequest, lisEnrollmentStopEnrollReq);

    lisEnrollmentStopEnrollRes = new LisEnrollmentStopEnrollRes(this);
    catcher1->subscribe(SIG_RIBD_StopEnrollmentResponse, lisEnrollmentStopEnrollRes);

    lisEnrollmentStartOperationReq = new LisEnrollmentStopOperationReq(this);
    catcher1->subscribe(SIG_RIBD_StartOperationRequest, lisEnrollmentStartOperationReq);

    lisEnrollmentStartOperationRes = new LisEnrollmentStartOperationRes(this);
    catcher1->subscribe(SIG_RIBD_StartOperationResponse, lisEnrollmentStartOperationRes);

    lisEnrollmentConResPosi = new LisEnrollmentConResPosi(this);
    catcher1->subscribe(SIG_RIBD_ConnectionResponsePositive, lisEnrollmentConResPosi);

    lisEnrollmentConResNega = new LisEnrollmentConResNega(this);
    catcher1->subscribe(SIG_RIBD_ConnectionResponseNegative, lisEnrollmentConResNega);

    lisEnrollmentConReq = new LisEnrollmentConReq(this);
    catcher1->subscribe(SIG_RIBD_ConnectionRequest, lisEnrollmentConReq);
}

void Enrollment::startCACE(const APNIPair &apnip) {
    Enter_Method("startCACE(%s)", apnip.str().c_str());
    EV_INFO << "Starting CACE phase" << endl;

    APNamingInfo src(apnip.first);
    APNamingInfo dst(apnip.second);

    // Check if we are already connected or in the process of connecting
    auto existingEntry = stateTable->findEntryByDstAPN(dst.getApn());
    if (existingEntry != nullptr) {
        auto status = existingEntry->getCACEConStatus();
        if (status != EnrollmentStateTableEntry::CON_NIL &&
            status != EnrollmentStateTableEntry::CON_ERROR) {
            EV << "Already in the process of enrolling with " << dst << endl;
            return;
        }
    }

    auto entry = EnrollmentStateTableEntry(src, dst, EnrollmentStateTableEntry::CON_AUTHENTICATING);
    stateTable->insert(entry);

    CDAP_M_Connect msg(MSG_CONREQ);

    authValue_t aValue;
    aValue.authName = authName;
    aValue.authPassword = authPassword;
    aValue.authOther = authOther;

    auth_t auth;
    auth.authType = authType;
    auth.authValue = aValue;

    msg.setAuth(auth);
    msg.setAbsSyntax(GPB);
    msg.setSrc(src);
    msg.setDst(dst);
    msg.setSrcAddr(Address(src.getApn()));
    msg.setDstAddr(Address(dst.getApn()));

    //send data to ribd to send
    signalizeCACESendData(&msg);
}

void Enrollment::insertStateTableEntry(Flow* flow){
    //insert only first flow created (management flow)
    if(stateTable->findEntryByDstAPN(APN(flow->getDstAddr().getApn().getName().c_str())) != nullptr) {
        return;
    }
    stateTable->insert(EnrollmentStateTableEntry(flow->getSrcApni(), flow->getDstApni(), EnrollmentStateTableEntry::CON_CONNECTPENDING));
}

void Enrollment::receivePositiveConnectResponse(CDAPMessage* msg) {
    Enter_Method("receivePositiveConnectResponse()");

    CDAP_M_Connect_R* cmsg = check_and_cast<CDAP_M_Connect_R*>(msg);
    EnrollmentStateTableEntry* entry = stateTable->findEntryByDstAPN(cmsg->getSrc().getApn());

    //check appropriate state
    if (entry->getCACEConStatus() != EnrollmentStateTableEntry::CON_AUTHENTICATING) {
        //TODO: send M_Release and invoke deallocate
        return;
    }

    entry->setCACEConStatus(EnrollmentStateTableEntry::CON_ESTABLISHED);

    startEnrollment(entry);
}

void Enrollment::receiveNegativeConnectResponse(CDAPMessage* msg) {
    Enter_Method("receiveNegativeConnectResponse()");

    CDAP_M_Connect_R* cmsg = check_and_cast<CDAP_M_Connect_R*>(msg);
    EnrollmentStateTableEntry* entry = stateTable->findEntryByDstAPN(cmsg->getSrc().getApn());

    //check appropriate state
    if (entry->getCACEConStatus() != EnrollmentStateTableEntry::CON_AUTHENTICATING) {
        //TODO: send M_Release and invoke deallocate
        return;
    }

    if (this->maxConRetries <= entry->getCurrentConnectRetries()) {
        entry->setCACEConStatus(EnrollmentStateTableEntry::CON_NIL);
        //TODO: send release and deallocate
        return;
    }


    entry->setCACEConStatus(EnrollmentStateTableEntry::CON_CONNECTPENDING);
    entry->increaseCurrentConnectRetries();
    //create and send new connect retry
    processNewConReq(entry);
}

void Enrollment::receiveConnectRequest(CDAPMessage* msg) {
    Enter_Method("receiveConnectRequest()");

    CDAP_M_Connect* cmsg = check_and_cast<CDAP_M_Connect*>(msg);

    stateTable->insert(EnrollmentStateTableEntry(cmsg->getDst(), cmsg->getSrc(),
                                                 EnrollmentStateTableEntry::CON_CONNECTPENDING));

    EnrollmentStateTableEntry* entry = stateTable->findEntryByDstAPN(cmsg->getSrc().getApn());

    if (!entry) {
        EV << "Enrollment status not found for "
           << cmsg->getSrc().getApn() << endl;
        return;
    }

    //what? we just inserted this
    //check appropriate state
    if (entry->getCACEConStatus() != EnrollmentStateTableEntry::CON_CONNECTPENDING) {
        //TODO: send M_Release and invoke deallocate
        return;
    }

    //check if message is valid
    if (cmsg->getAbsSyntax() != GPB) {
        this->processConResNega(entry, cmsg);
        return;
    }

    entry->setCACEConStatus(EnrollmentStateTableEntry::CON_AUTHENTICATING);

    authenticate(entry, cmsg);
}

/*   enrollment initiator */

void Enrollment::startEnrollment(EnrollmentStateTableEntry* entry) {
    Enter_Method("startEnrollment()");

    auto enrollObj = new EnrollmentObj(Address(entry->getLocal().getApn()), Address(entry->getRemote().getApn()));

    enrollObj->setAddress(ribDaemon->getMyAddress().getIpcAddress());

    //TODO: add other necessary information

    //process enrollment object to ribd to send
    signalizeStartEnrollmentRequest(enrollObj);

    //set appropriate state
    entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_WAIT_START_RESPONSE_ENROLLMENT);
}

void Enrollment::receiveStartEnrollmentResponse(CDAPMessage* msg) {
    Enter_Method("receiveStartEnrollmentResponse()");

    CDAP_M_Start_R* smsg = check_and_cast<CDAP_M_Start_R*>(msg);

    //not expected message
    if (!smsg) {
        //TODO: send release and deallocate
        return;
    }

    EnrollmentObj* enrollRec = check_and_cast<EnrollmentObj*>(smsg->getObjectItem().objectVal);
    EnrollmentStateTableEntry* entry = stateTable->findEntryByDstAPN(enrollRec->getSrcAddress().getApn());

    //check for appropriate state
    if (entry->getEnrollmentStatus() != EnrollmentStateTableEntry::ENROLL_WAIT_START_RESPONSE_ENROLLMENT) {
        //TODO: send release and deallocate
        return;
    }

    //assign new, received address
    Address newAddr = ribDaemon->getMyAddress();
    newAddr.setIpcAddress(APN(enrollRec->getAddress().getName()));
    ribDaemon->setMyAddress(newAddr);

    //TODO: assign other received values

    //change state
    entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_WAIT_STOP_ENROLLMENT);

    //TODO: wait for create messages and stop enrollment request
    delete enrollRec;
}

void Enrollment::receiveStopEnrollmentRequest(CDAPMessage* msg) {
    Enter_Method("receiveStopEnrollmentRequest()");


    CDAP_M_Stop* smsg = check_and_cast<CDAP_M_Stop*>(msg);

    //not expected message
    if (!smsg) {
        //TODO: send release and deallocate
        return;
    }

    EnrollmentObj* enrollRec = check_and_cast<EnrollmentObj*>(smsg->getObjectItem().objectVal);
    EnrollmentStateTableEntry* entry = stateTable->findEntryByDstAPN(enrollRec->getSrcAddress().getApn());

    //check for appropriate state
    if (entry->getEnrollmentStatus() != EnrollmentStateTableEntry::ENROLL_WAIT_STOP_ENROLLMENT) {
        //TODO: send release and deallocate
        return;
    }

    //set immediate transition to operational state
    entry->setIsImmediateEnrollment(enrollRec->getIsImmediate());

    //set appropriate state
    entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_WAIT_READ_RESPONSE);

    //TODO: send read requests, wait for responses, send Mstop enrollment
    //for now send stop enrollment response
    processStopEnrollmentResponse(entry);
    delete enrollRec;
}

void Enrollment::processStopEnrollmentResponse(EnrollmentStateTableEntry* entry) {

    auto enrollObj = new EnrollmentObj(Address(entry->getLocal().getApn()), Address(entry->getRemote().getApn()));

    signalizeStopEnrollmentResponse(enrollObj);

    if (entry->getIsImmediateEnrollment()) {
        entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_ENROLLED);
        updateEnrollmentDisplay(ENICON_ENROLLED);
        APNIPair apnip(entry->getLocal(), entry->getRemote());
        flowAlloc->receiveMgmtAllocateFinish(&apnip);
    }
    else {
        entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_WAIT_START_OPERATION);
        //TODO: continue enrollment here
    }
}

void Enrollment::receiveStartOperationRequest(CDAPMessage* msg) {
    Enter_Method("receiveStartOperationRequest()");
    delete msg;
    EV_ERROR << __func__ << " is not implemented" << endl;
}

/* enrollment member */

void Enrollment::receiveStartEnrollmentRequest(CDAPMessage* msg) {
    Enter_Method("receiveStartEnrollmentRequest()");

    CDAP_M_Start* smsg = check_and_cast<CDAP_M_Start*>(msg);

    //not expected message
    if (!smsg) {
        //TODO: send release and deallocate
        return;
    }

    EnrollmentObj* enrollRec = check_and_cast<EnrollmentObj*>(smsg->getObjectItem().objectVal);
    EnrollmentStateTableEntry* entry = stateTable->findEntryByDstAPN(enrollRec->getSrcAddress().getApn().getName());

    //check for appropriate state
    if (entry->getEnrollmentStatus() != EnrollmentStateTableEntry::ENROLL_WAIT_START_ENROLLMENT) {
        //TODO: send release and deallocate
        return;
    }


    auto enrollObj = new EnrollmentObj(Address(entry->getLocal().getApn()), Address(entry->getRemote().getApn()));

    //TODO: repair this dummy address assign
    enrollObj->setAddress(APN(enrollRec->getAddress().getName()));

    //TODO: add other necessary information

    //process enrollment object to ribd to send
    signalizeStartEnrollmentResponse(enrollObj);

    //TODO: send create messages, wait for responses, then send stop enrollment
    //for now send stop enrollment
    processStopEnrollmentImmediate(entry);
    delete enrollRec;
}

void Enrollment::receiveStopEnrollmentResponse(CDAPMessage* msg) {
    Enter_Method("receiveStopEnrollmentResponse()");

    CDAP_M_Stop_R* smsg = dynamic_cast<CDAP_M_Stop_R*>(msg);
    //not expected message
    if (!smsg) {
        //TODO: send release and deallocate
        return;
    }

    EnrollmentObj* enrollRec = check_and_cast<EnrollmentObj*>(smsg->getObjectItem().objectVal);
    EnrollmentStateTableEntry* entry = stateTable->findEntryByDstAPN(enrollRec->getSrcAddress().getApn());

    //check for appropriate state
    if (entry->getEnrollmentStatus() != EnrollmentStateTableEntry::ENROLL_WAIT_STOP_RESPONSE_ENROLLMENT) {
        //TODO: send release and deallocate
        return;
    }

    if (entry->getIsImmediateEnrollment()) {
        entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_ENROLLED);
        //TODO: emit signal somewhere and probably send rib update ...
    }
    else {
        //TODO: add appropriate state for read and wait operation
    }
    delete enrollRec;
}

void Enrollment::receiveStartOperationResponse(CDAPMessage* msg) {
    Enter_Method("receiveStartOperationResponse()");
    delete msg;
    EV_ERROR << __func__ << " is not implemented" << endl;
}

void Enrollment::processStopEnrollmentImmediate(EnrollmentStateTableEntry* entry) {
    auto enrollObj = new EnrollmentObj(Address(entry->getLocal().getApn()), Address(entry->getRemote().getApn()));

    //set immediate
    enrollObj->setIsImmediate(true);
    entry->setIsImmediateEnrollment(true);

    //TODO: add other necessary information

    signalizeStopEnrollmentRequest(enrollObj);

    entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_WAIT_STOP_RESPONSE_ENROLLMENT);
}

void Enrollment::authenticate(EnrollmentStateTableEntry* entry, CDAP_M_Connect* msg) {
    Enter_Method("authenticate()");

    //check and validate expected auth type
    if (msg->getAuth().authType == authType) {
        //none auth option
        if (msg->getAuth().authType == AUTH_NONE) {
            processConResPosi(entry, msg);
            return;

        }
        //passwd auth option
        else if (msg->getAuth().authType == AUTH_PASSWD) {
            //correct passwd
            if (!strcmp(msg->getAuth().authValue.authPassword.c_str(), authPassword.c_str())) {
                processConResPosi(entry, msg);
                return;
            }
        }
        //TODO: support for other options
    }

    //not valid auth send negative response
    processConResNega(entry, msg);
}

void Enrollment::processNewConReq(EnrollmentStateTableEntry* entry) {
    Enter_Method("processNewConReq()");

    //TODO: probably change values, this is retry

    CDAP_M_Connect msg(MSG_CONREQRETRY);

    authValue_t aValue;
    aValue.authName = authName;
    aValue.authPassword = authPassword;
    aValue.authOther = authOther;

    auth_t auth;
    auth.authType = authType;
    auth.authValue = aValue;

    msg.setAuth(auth);
    msg.setAbsSyntax(GPB);

    APNamingInfo src = APNamingInfo(entry->getLocal().getApn(),
                entry->getLocal().getApinstance(),
                entry->getLocal().getAename(),
                entry->getLocal().getAeinstance());

    APNamingInfo dst = APNamingInfo(entry->getRemote().getApn(),
            entry->getRemote().getApinstance(),
            entry->getRemote().getAename(),
            entry->getRemote().getAeinstance());

    msg.setSrc(src);
    msg.setDst(dst);

    msg.setSrcAddr(Address(entry->getLocal().getApn()));
    msg.setDstAddr(Address(entry->getRemote().getApn()));

    //send data to ribd to send
    signalizeCACESendData(&msg);

    //change state to auth after send retry
    entry->setCACEConStatus(EnrollmentStateTableEntry::CON_AUTHENTICATING);
}

void Enrollment::processConResPosi(EnrollmentStateTableEntry* entry, CDAPMessage* cmsg) {
    CDAP_M_Connect_R msg(MSG_CONRESPOS);
    CDAP_M_Connect* cmsg1 = check_and_cast<CDAP_M_Connect*>(cmsg);

    APNamingInfo src = APNamingInfo(entry->getLocal().getApn(),
                entry->getLocal().getApinstance(),
                entry->getLocal().getAename(),
                entry->getLocal().getAeinstance());

    APNamingInfo dst = APNamingInfo(entry->getRemote().getApn(),
            entry->getRemote().getApinstance(),
            entry->getRemote().getAename(),
            entry->getRemote().getAeinstance());

    result_t result;
    result.resultValue = R_SUCCESS;

    auth_t auth;
    auth.authType = cmsg1->getAuth().authType;
    auth.authValue = cmsg1->getAuth().authValue;

    msg.setAbsSyntax(GPB);
    msg.setResult(result);
    msg.setAuth(auth);

    msg.setSrc(src);
    msg.setDst(dst);

    msg.setSrcAddr(Address(entry->getLocal().getApn()));
    msg.setDstAddr(Address(entry->getRemote().getApn()));

    //send data to ribd to send
    signalizeCACESendData(&msg);

    entry->setCACEConStatus(EnrollmentStateTableEntry::CON_ESTABLISHED);
    entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_WAIT_START_ENROLLMENT);
}

void Enrollment::processConResNega(EnrollmentStateTableEntry* entry, CDAPMessage* cmsg) {
    CDAP_M_Connect_R msg(MSG_CONRESNEG);
    CDAP_M_Connect* cmsg1 = check_and_cast<CDAP_M_Connect*>(cmsg);

    APNamingInfo src = APNamingInfo(entry->getLocal().getApn(),
                entry->getLocal().getApinstance(),
                entry->getLocal().getAename(),
                entry->getLocal().getAeinstance());

    APNamingInfo dst = APNamingInfo(entry->getRemote().getApn(),
            entry->getRemote().getApinstance(),
            entry->getRemote().getAename(),
            entry->getRemote().getAeinstance());

    result_t result;
    result.resultValue = R_FAIL;

    auth_t auth;
    auth.authType = cmsg1->getAuth().authType;
    auth.authValue = cmsg1->getAuth().authValue;

    msg.setAbsSyntax(GPB);
    msg.setResult(result);
    msg.setAuth(auth);

    msg.setSrc(src);
    msg.setDst(dst);

    msg.setSrcAddr(Address(entry->getLocal().getApn()));
    msg.setDstAddr(Address(entry->getRemote().getApn()));

    //send data to send to ribd
    signalizeCACESendData(&msg);

    entry->setCACEConStatus(EnrollmentStateTableEntry::CON_CONNECTPENDING);

    //increase number of connects
    entry->increaseCurrentConnectRetries();
}

void Enrollment::signalizeCACESendData(CDAPMessage* cmsg) {
    emit(sigEnrollmentCACESendData, cmsg);
}

void Enrollment::signalizeStartEnrollmentRequest(EnrollmentObj* obj) {
    emit(sigEnrollmentStartEnrollReq, obj);
}

void Enrollment::signalizeStartEnrollmentResponse(EnrollmentObj* obj) {
    emit(sigEnrollmentStartEnrollRes, obj);
}

void Enrollment::signalizeStopEnrollmentRequest(EnrollmentObj* obj) {
    emit(sigEnrollmentStopEnrollReq, obj);
}

void Enrollment::signalizeStopEnrollmentResponse(EnrollmentObj* obj) {
    emit(sigEnrollmentStopEnrollRes, obj);
}

void Enrollment::signalizeStartOperationRequest(OperationObj* obj) {
    emit(sigEnrollmentStartOperReq, obj);
}

void Enrollment::signalizeStartOperationResponse(OperationObj* obj) {
    emit(sigEnrollmentStartOperRes, obj);
}

void Enrollment::parseConfig(cXMLElement *config)
{
    if (config == nullptr || !config->hasChildren()) {
        EV_INFO << "configData parameter not initialized!" << endl;
        return;
    }

    const auto &mainTag = config->getFirstChildWithTag(ELEM_PREENROL);
    if (mainTag == nullptr) {
        EV_INFO << "Configuration has no preenrollment tags" << endl;
        return;
    }

    const auto &enrll = mainTag->getChildrenByTagName(ELEM_SIMTIME);
    for (const auto &m : enrll) {
        const auto &timeAttr = m->getAttribute(ATTR_TIME);
        if (timeAttr == nullptr) {
            EV_WARN << "SimTime tag is missing time attribute!" << endl;
            continue;
        }

        simtime_t cas = SimTime::parse(timeAttr);
        const auto &connections = m->getChildrenByTagName(ELEM_CONNECT);
        if (!connections.empty()) {
            preConnects[cas] = new NameQueue;
            cMessage *cmsg = new cMessage(MSG_ENRLCON);
            scheduleAt(cas, cmsg);
            for (const auto &conn : connections) {
                const char *src = conn->getAttribute(ATTR_SRC);
                const char *dst = conn->getAttribute(ATTR_DST);
                if (src == nullptr || dst == nullptr) {
                    EV_WARN << "Error parsing preenrollment connection tag, needs "
                            << "both source and destination addresses" << endl;
                    continue;
                }

                preConnects[cas]->push(APNIPair(src, dst));
            }
        }

        const auto &releases = m->getChildrenByTagName(ELEM_RELEASE);
        if (!releases.empty()) {
            preReleases[cas] = new NameQueue;
            cMessage *cmsg = new cMessage(MSG_ENRLREL);
            scheduleAt(cas, cmsg);
            for (const auto &rel : releases) {
                const char *src = rel->getAttribute(ATTR_SRC);
                const char *dst = rel->getAttribute(ATTR_DST);
                if (src == nullptr || dst == nullptr) {
                    EV_WARN << "Error parsing preenrollment release tag, needs "
                            << "both source and destination addresses" << endl;
                    continue;
                }

                preReleases[cas]->push(APNIPair(src, dst));
            }
        }
    }
}

void Enrollment::updateEnrollmentDisplay(Enrollment::IconEnrolStatus status) {
    cModule* ipc = this->getModuleByPath("^.^");
    std::string ico, col;
    switch (status) {
        case ENICON_ENROLLED: {ico="status/check"; col="green"; break;}
        case ENICON_FLOWMIS: {ico="status/excl"; col="yellow";break;}
        case ENICON_NOTENROLLED:
        default:              {ico="status/cross"; col="red"; break;}

    }
    ipc->getDisplayString().setTagArg("i2", 0, ico.c_str());
    ipc->getDisplayString().setTagArg("i2", 1, col.c_str());
}

void Enrollment::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        simtime_t now = simTime();
        if (!opp_strcmp(msg->getName(), MSG_ENRLCON)) {
            auto &apniPairs = preConnects[now];
            EV << "Preallocation at time " << simTime() << " has " << apniPairs->size()
               << " elements" << endl;
            while (!apniPairs->empty()) {
                APNIPair pair = apniPairs->front();
                auto entry = stateTable->findEntryByDstAPN(pair.second.getApn());
                if (!entry) {
                    flowAlloc->receiveMgmtAllocateRequest(pair.first, pair.second);
                }
                apniPairs->pop();
            }

            delete apniPairs;
            preConnects.erase(now);
        } else if (!opp_strcmp(msg->getName(), MSG_ENRLREL)) {
            auto &apniPairs = preReleases[simTime()];
            while (!apniPairs->empty()) {
                APNIPair pair = apniPairs->front();
                auto entry = stateTable->findEntryByDstAPN(pair.second.getApn());
                if (entry &&
                    entry->getEnrollmentStatus() == EnrollmentStateTableEntry::ENROLL_ENROLLED) {
                    // FIXME: Vesely->Jerabek: Here goes release part of Enrollment
                }
                apniPairs->pop();
            }

            delete apniPairs;
            preReleases.erase(now);
        }
        delete msg;
    }
}
