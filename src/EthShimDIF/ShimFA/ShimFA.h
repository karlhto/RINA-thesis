#ifndef RINASIM_SHIMFA_H
#define RINASIM_SHIMFA_H

#include <omnetpp.h>
#include "DIF/FA/FABase.h"

class ShimFA : public FABase {
public:
    ShimFA();
    virtual ~ShimFA();

    virtual bool receiveAllocateRequest(Flow* flow);
    virtual bool receiveMgmtAllocateRequest(Flow* mgmtflow);
    virtual bool receiveMgmtAllocateRequest(APNamingInfo src, APNamingInfo dst);
    virtual bool receiveMgmtAllocateFinish();
    virtual void receiveNM1FlowCreated(Flow* flow);
    virtual bool receiveCreateFlowRequestFromRibd(Flow* flow);
    virtual bool receiveDeallocateRequest(Flow* flow);
    virtual void deinstantiateFai(Flow* flow);
    virtual bool invokeNewFlowRequestPolicy(Flow* flow);

    virtual bool setOriginalAddresses(Flow* flow);
    virtual bool setNeighborAddresses(Flow* flow);

    NFlowTable* getNFlowTable() const;
    const Address& getMyAddress() const;

protected:
    NFlowTable* N_flowTable;
    Address MyAddress;

    //SimpleModule overloads
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    void initMyAddress();
};

#endif //RINASIM_SHIMFA_H
