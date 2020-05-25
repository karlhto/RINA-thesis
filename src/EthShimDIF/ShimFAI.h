#ifndef RINASIM_SHIMFAI_H
#define RINASIM_SHIMFAI_H

#include "DIF/FA/FAIBase.h"

class ShimFAI : public FAIBase {
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif //RINASIM_SHIMFAI_H
