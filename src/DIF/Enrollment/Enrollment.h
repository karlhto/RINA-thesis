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
 * @file Enrollment.h
 * @author Kamil Jerabek (xjerab18@stud.fit.vutbr.cz)
 * @date Apr 1, 2015
 * @brief Enrollment and CACE
 * @detail
 */

#ifndef __RINA_ENROLLMENT_H_
#define __RINA_ENROLLMENT_H_

#include <omnetpp.h>
#include <memory>
#include <queue>

extern const char* MSG_CONREQ;
extern const char* MSG_CONREQRETRY;
extern const char* MSG_CONRESPOS;
extern const char* MSG_CONRESNEG;
extern const char* MSG_ENRLCON;
extern const char* MSG_ENRLREL;

class LisEnrollmentAllResPosi;
class LisEnrollmentGetFlowFromFaiCreResPosi;
class LisEnrollmentStartEnrollReq;
class LisEnrollmentStartEnrollRes;
class LisEnrollmentStopEnrollReq;
class LisEnrollmentStopEnrollRes;
class LisEnrollmentStopOperationReq;
class LisEnrollmentStartOperationRes;
class LisEnrollmentConResPosi;
class LisEnrollmentConResNega;
class LisEnrollmentConReq;

class APNIPair;
class FABase;
class EnrollmentStateTable;
class EnrollmentStateTableEntry;
class RIBd;
class CDAPMessage;
class Flow;
class EnrollmentObj;
class OperationObj;
class CDAP_M_Connect;

class Enrollment : public cSimpleModule
{
  private:
    using NameQueue = std::queue<APNIPair>;
    using EnrollmentCommands = std::map<simtime_t, NameQueue *>;

    EnrollmentCommands preConnects;
    EnrollmentCommands preReleases;

    int authType;
    std::string authName;
    std::string authPassword;
    std::string authOther;
    int maxConRetries;
    int numOfConnects;

    FABase* flowAlloc;
    EnrollmentStateTable* stateTable;
    RIBd* ribDaemon;

    simsignal_t sigEnrollmentCACESendData;
    simsignal_t sigEnrollmentSendData;
    simsignal_t sigEnrollmentStartEnrollReq;
    simsignal_t sigEnrollmentStartEnrollRes;
    simsignal_t sigEnrollmentStopEnrollReq;
    simsignal_t sigEnrollmentStopEnrollRes;
    simsignal_t sigEnrollmentStartOperReq;
    simsignal_t sigEnrollmentStartOperRes;

    // TODO (someone): Please kill these with fire
    LisEnrollmentAllResPosi* lisEnrollmentAllResPosi = nullptr;
    LisEnrollmentGetFlowFromFaiCreResPosi* lisEnrollmentGetFlowFromFaiCreResPosi = nullptr;
    LisEnrollmentStartEnrollReq* lisEnrollmentStartEnrollReq = nullptr;
    LisEnrollmentStartEnrollRes* lisEnrollmentStartEnrollRes = nullptr;
    LisEnrollmentStopEnrollReq* lisEnrollmentStopEnrollReq = nullptr;
    LisEnrollmentStopEnrollRes* lisEnrollmentStopEnrollRes = nullptr;
    LisEnrollmentStopOperationReq* lisEnrollmentStartOperationReq = nullptr;
    LisEnrollmentStartOperationRes* lisEnrollmentStartOperationRes = nullptr;
    LisEnrollmentConResPosi* lisEnrollmentConResPosi = nullptr;
    LisEnrollmentConResNega* lisEnrollmentConResNega = nullptr;
    LisEnrollmentConReq* lisEnrollmentConReq = nullptr;

  public:
    enum IconEnrolStatus {ENICON_ENROLLED, ENICON_FLOWMIS, ENICON_NOTENROLLED};

  private:
    void initPointers();
    void initSignalsAndListeners();

    void updateEnrollmentDisplay(Enrollment::IconEnrolStatus status);

    void parseConfig(cXMLElement* config);

    void authenticate(EnrollmentStateTableEntry* entry, CDAP_M_Connect* msg);
    void processConResPosi(EnrollmentStateTableEntry* entry, CDAPMessage* cmsg);
    void processConResNega(EnrollmentStateTableEntry* entry, CDAPMessage* cmsg);
    void processNewConReq(EnrollmentStateTableEntry* entry);
    void processStopEnrollmentImmediate(EnrollmentStateTableEntry* entry);
    void processStopEnrollmentResponse(EnrollmentStateTableEntry* entry);

    void signalizeCACESendData(CDAPMessage* cmsg);
    void signalizeStartEnrollmentRequest(EnrollmentObj* obj);
    void signalizeStartEnrollmentResponse(EnrollmentObj* obj);
    void signalizeStopEnrollmentRequest(EnrollmentObj* obj);
    void signalizeStopEnrollmentResponse(EnrollmentObj* obj);
    void signalizeStartOperationRequest(OperationObj* obj);
    void signalizeStartOperationResponse(OperationObj* obj);

    // cSimpleModule overrides
    void handleMessage(cMessage *msg) override;
    void initialize() override;

  public:
    Enrollment() = default;
    ~Enrollment() override;
    void startCACE(const APNIPair &apnip);
    void startEnrollment(EnrollmentStateTableEntry* entry);
    void insertStateTableEntry(Flow* flow);
    void receivePositiveConnectResponse(CDAPMessage* msg);
    void receiveNegativeConnectResponse(CDAPMessage* msg);
    void receiveConnectRequest(CDAPMessage* msg);

    void receiveStartEnrollmentRequest(CDAPMessage* msg);
    void receiveStartEnrollmentResponse(CDAPMessage* msg);
    void receiveStopEnrollmentRequest(CDAPMessage* msg);
    void receiveStopEnrollmentResponse(CDAPMessage* msg);
    void receiveStartOperationRequest(CDAPMessage* msg);
    void receiveStartOperationResponse(CDAPMessage* msg);
};

#endif
