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

package rina.examples.Shim.ThreeHostsShim;

import rina.src.CS.EthShim.Host1APShim;
import rina.src.CS.EthShim.Host1AP2Shim;
import inet.node.ethernet.Eth100M;


network ThreeHostsShim
{
    submodules:
        host1: Host1APShim {
            @display("p=82,92");
        }
        host2: Host1AP2Shim {
            @display("p=231,92");
        }
        host3: Host1APShim {
            @display("p=380,92");
        }
    connections allowunconnected:
        host1.medium <--> Eth100M <--> host2.medium[0];
        host2.medium[1] <--> Eth100M <--> host3.medium;
}
