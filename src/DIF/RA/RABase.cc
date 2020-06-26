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

#include "DIF/RA/RABase.h"

#include "Common/RINASignals.h"

// QoS loader parameters
const char* PAR_QOSDATA              = "qoscubesData";
const char* ELEM_QOSCUBE             = "QoSCube";
const char* PAR_QOSREQ               = "qosReqData";
const char* ELEM_QOSREQ              = "QoSReq";
const char* ATTR_ID                  = "id";

const simsignal_t RABase::createFlowPositiveSignal = registerSignal(SIG_RA_CreateFlowPositive);
const simsignal_t RABase::createFlowNegativeSignal = registerSignal(SIG_RA_CreateFlowNegative);
const simsignal_t RABase::invokeSlowdownSignal = registerSignal(SIG_RA_InvokeSlowdown);
const simsignal_t RABase::executeSlowdownSignal = registerSignal(SIG_RA_ExecuteSlowdown);

RABase::RABase() : QoSCubes() {}

RABase::~RABase() {}

void RABase::initialize(int stage) {
    if (stage == 0)
        initQoSCubes();
}

void RABase::initQoSCubes()
{
    cXMLElement* qosXml = nullptr;
    if (par(PAR_QOSDATA).xmlValue() != nullptr
            && par(PAR_QOSDATA).xmlValue()->hasChildren())
        qosXml = par(PAR_QOSDATA).xmlValue();
    else
        error("qoscubesData parameter not initialized!");

    // load cubes from XML
    cXMLElementList cubes = qosXml->getChildrenByTagName(ELEM_QOSCUBE);
    for (auto const m : cubes)
    {
        if (!m->getAttribute(ATTR_ID))
        {
            EV << "Error parsing QoSCube. Its ID is missing!" << endl;
            continue;
        }

        cXMLElementList attrs = m->getChildren();
        QoSCube cube = QoSCube(attrs);
        cube.setQosId(m->getAttribute(ATTR_ID));

        //Integrity check!!!
        if (cube.isDefined())
        {
            QoSCubes.push_back(cube);
        }
        else
        {
            EV << "QoSCube with ID " << cube.getQosId()
                    << " contains DO-NOT-CARE parameter. It is not fully defined,"
                    << " thus it is not loaded into RA's QoS-cube set!"
                    << endl;
        }
    }

    if (!QoSCubes.size())
    {
        std::ostringstream os;
        os << this->getFullPath()
                << " does not have any QoSCube in its set. "
                << "It cannot work without at least one valid QoS cube!"
                << endl;
        error(os.str().c_str());
    }
}

void RABase::handleMessage(cMessage *msg) {
    delete msg;
    throw cRuntimeError("This module is not supposed to handle messages");
}

const QoSCubeSet& RABase::getQoSCubes() const {
    return QoSCubes;
}



std::ostream& operator <<(std::ostream& os, const QoSCubeSet& cubes) {
    for (auto const& it : cubes)
    {
        os << it;
    }
    return os;
}

const QoSCube* RABase::getQoSCubeById(const std::string &qosId) const
{
    for (auto const& it : QoSCubes)
    {
        if (!it.getQosId().compare(qosId))
        {
            return &it;
        }
    }
    return nullptr;
}
