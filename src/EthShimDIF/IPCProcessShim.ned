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

import inet.common.MessageDispatcher;
import inet.linklayer.contract.IEthernetInterface;
import inet.networklayer.common.InterfaceTable;

module IPCProcessShim
{
    parameters:
        @display("bgb=402,300;i=misc/square,#008000,50;i2=status/connect");
        string ipcAddress;
        string difName;
        string apName = default(ipcAddress + "_" + difName);
        *.interfaceTableModule = default(absPath(".interfaceTable"));

    gates:
        inout northIo;
        inout phys;
    submodules:
        shim: EthShim {
            parameters:
                @display("p=201,127");
        }

        flowAllocator: ShimFlowAllocator {
            parameters:
                @display("p=320,43");
        }

        resourceAllocator: ShimResourceAllocator {
            parameters:
                @display("p=320,127");
        }

        arp: RINArp {
            parameters:
                @display("p=82,127");
        }

        sl: MessageDispatcher {
            parameters:
                @display("p=201,184;b=200,5,,,,1");
        }

        interfaceTable: InterfaceTable {
            parameters:
                @display("p=320,232;is=s");
        }

        eth: <default("EthernetInterface")> like IEthernetInterface {
            parameters:
                @display("p=201,240,row,60;q=txQueue");
                qEncap.typename = "Ieee8021qEncap";
                // Ignore warnings about parameters not existing, these are part of
                // Ieee8021qEncap when initialised.
                qEncap.inboundVlanIdMap = difName + " -1";
                qEncap.outboundVlanIdMap = "-1 " + difName;
        }

    connections allowunconnected:
        arp.ifOut --> sl.in++;
        arp.ifIn <-- sl.out++;
        shim.ifOut --> sl.in++;
        shim.ifIn <-- sl.out++;
        eth.upperLayerOut --> sl.in++;
        eth.upperLayerIn <-- sl.out++;
        eth.phys <--> { @display("m=s"); } <--> phys;
}
