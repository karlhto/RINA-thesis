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

#ifndef MESSAGELOGENTRY_H_
#define MESSAGELOGENTRY_H_

//Standard libraries
#include <omnetpp.h>
//RINASim libraries
#include "DAF/CDAP/CDAPMessage_m.h"
#include "Common/ExternConsts.h"

class CDAPMsgLogEntry {
  public:


    //(De)Constructor
    CDAPMsgLogEntry(unsigned char opc, long invoke, bool srflag);
    virtual ~CDAPMsgLogEntry();

    std::string str() const;
    std::string getOpCodeString() const;

    //Getters and Setters
    long getInvokeId() const;
    void setInvokeId(long invokeId);

    unsigned char getOpCode() const;
    void setOpCode(const unsigned char opCode);

    const simtime_t& getProcessedAt() const;
    void setProcessedAt(const simtime_t& requestedAt);

  private:
    //FIXME: Vesely - Convert to enum
    unsigned char opCode;
    long invokeId;
    simtime_t processedAt;
    bool sndFlag;

};

//Free function
std::ostream& operator<< (std::ostream& os, const CDAPMsgLogEntry& mle);

#endif /* MESSAGELOGENTRY_H_ */
