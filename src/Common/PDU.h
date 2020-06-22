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
 * @file PDU.h
 * @author Marcel Marek (imarek@fit.vutbr.cz)
 * @date Apr 30, 2014
 * @brief
 * @detail
 */

#ifndef PDU_H_
#define PDU_H_


#define SDU_FRAG_LEN 2 //number of bytes for length field of a fragment
#define SDU_DELIMITER_FLAG_LEN 1 //size of SDUDelimiterFlags in bytes
#define DRF_FLAG 0x80
#define ECN_FLAG 0x01

#include <omnetpp.h>
#include "Common/PDU_m.h"

class PDU : public PDU_Base
{
  public:
    PDU(const char *name = NULL, int kind = 0) : PDU_Base(name, kind) {}

    virtual PDU *dup() const { return new PDU(*this); }
    // ADD CODE HERE to redefine and implement pure virtual functions from PDU_Base

    unsigned int getHeaderSize();
    unsigned int getSize();
};

#endif /* PDU_H_ */
