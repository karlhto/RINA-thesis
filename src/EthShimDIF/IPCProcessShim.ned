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

import rina.EthShimDIF.ShimFA.FlowAllocator;
import inet.linklayer.contract.IWiredNic;

module IPCProcessShim
{
    parameters:
        @display("bgb=457,307;i=misc/square,#008000,50");
        string ipcAddress;
        string difName;
        string apName = default(ipcAddress + "_" + difName);
    gates:
        inout northIo;
        inout ethg;
    submodules:
        shim: EthShim {
            parameters:
                @display("p=100,0");
        }
        // This needs to look like a normal flow allocator to DA
        flowAllocator: ShimFlowAllocator {
            parameters:
                @display("p=100,100");
        }
        arp: RINArp {
            parameters:
                @display("p=50,0");
        }
        eth: <default("EthernetInterface")> like IWiredNic {
            parameters:
                @display("p=368,406,row,60;q=txQueue");
        }
    connections allowunconnected:
        shim.northIo <--> northIo;
        shim.arpOut --> arp.netwIn;
        shim.arpIn <-- arp.netwOut;
        shim.ifOut --> eth.upperLayerIn;
        shim.ifIn <-- eth.upperLayerOut;
        eth.phys <--> { @display("m=s"); } <--> ethg;
}
