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
 * @file Data.h
 * @author Marcel Marek (imarek@fit.vutbr.cz)
 * @date Sep 18, 2014
 * @brief
 * @detail
 */

#ifndef DATA_H_
#define DATA_H_

#include <omnetpp.h>
#include "DIF/Delimiting/Data_m.h"

class Data : public Data_Base
  {
    private:
      void copy(const Data& other) {};
    public:
     Data(const char *name=nullptr, int kind=0) : Data_Base(name,kind) {}
     Data(const Data& other) : Data_Base(other) {copy(other);}
     Data& operator=(const Data& other) {if (this==&other) return *this; Data_Base::operator=(other); copy(other); return *this;}
     virtual Data *dup() const {return new Data(*this);}
      // ADD CODE HERE to redefine and implement pure virtual functions from Data_Base
  };


#endif /* DATA_H_ */
