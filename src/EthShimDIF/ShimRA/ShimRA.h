#include <omnetpp.h>

#include "DIF/RA/RABase.h"

class ShimRA : public RABase
{
  protected:
  public:
    ShimRA();
    virtual ~ShimRA();

    virtual void createNM1Flow(Flow *flow);
    virtual void createNM1FlowWithoutAllocate(Flow *flow);
    virtual void createNFlow(Flow *flow);
    virtual void postNFlowAllocation(Flow *flow);
    virtual void postNM1FlowAllocation(NM1FlowTableItem *ftItem);
    virtual void removeNM1Flow(Flow *flow);
    virtual void removeNM1FlowBindings(NM1FlowTableItem *ftItem);
    virtual bool bindNFlowToNM1Flow(Flow *flow);
    virtual void blockNM1PortOutput(NM1FlowTableItem *ftItem);
    virtual void unblockNM1PortOutput(NM1FlowTableItem *ftItem);
    virtual NM1FlowTable *getFlowTable();
    virtual void signalizeSlowdownRequestToRIBd(cPacket *pdu);
    virtual void signalizeSlowdownRequestToEFCP(cObject *pdu);
    virtual bool hasFlow(std::string addr, std::string qosId);

  protected:
    /// cSimpleModule overrides
    virtual void handleMessage(cMessage *msg) override;
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return 2; }
};
