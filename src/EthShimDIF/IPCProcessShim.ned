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

package rina.src.EthShimDIF;

import rina.src.EthShimDIF.ShimFA.ShimFlowAllocator;
import rina.src.EthShimDIF.ShimRA.ShimResourceAllocator;
import rina.src.EthShimDIF.EthShim.EthShim;
import rina.src.EthShimDIF.RINArp.RINArp;

module IPCProcessShim
{
    parameters:
        @display("bgb=402,307;i=misc/square,#008000,50");
        string ipcAddress;
        string difName;
        string apName = default(ipcAddress + "_" + difName);
        string interfaceTableModule;
        *.interfaceTableModule = default(absPath(interfaceTableModule));

    gates:
        inout northIo;
        input ifIn;
        output ifOut;
    submodules:
        shim: EthShim {
            parameters:
                @display("p=201,127");
        }

        flowAllocator: ShimFlowAllocator {
            parameters:
                @display("p=346,48");
        }

        resourceAllocator: ShimResourceAllocator {
            parameters:
                @display("p=346,127");
        }

        arp: RINArp {
            parameters:
                @display("p=55,127");
        }

    connections allowunconnected:
        arp.netwOut --> shim.arpIn;
        arp.netwIn <-- shim.arpOut;
        shim.ifOut --> { @display("m=s"); } --> ifOut;
        shim.ifIn <-- { @display("m=s"); } <-- ifIn;
}
