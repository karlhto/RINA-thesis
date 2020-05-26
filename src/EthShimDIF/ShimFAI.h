#ifndef RINASIM_SHIMFAI_H
#define RINASIM_SHIMFAI_H

#include "DIF/FA/FAIBase.h"
#include "DIF/FA/FABase.h"

class ShimFAI : public FAIBase {
public:
    ShimFAI();
    virtual ~ShimFAI();

    virtual bool receiveAllocateRequest();
    virtual bool receiveAllocateResponsePositive();
    virtual void receiveAllocateResponseNegative();
    virtual bool receiveCreateRequest();
    virtual bool receiveCreateResponsePositive(Flow* flow);
    virtual bool receiveCreateResponseNegative();
    virtual bool receiveDeallocateRequest();
    virtual void receiveDeleteRequest(Flow* flow);
    virtual void receiveDeleteResponse();

    virtual void receiveCreateFlowResponsePositiveFromNminusOne();
    virtual void receiveCreateFlowResponseNegativeFromNminusOne();

    const FABase* getFa() const {
        return FaModule;
    }

protected:
    FABase *FaModule;
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif //RINASIM_SHIMFAI_H
