
package rina.src.CS;

import rina.src.DAF.ApplicationProcess;
import rina.src.DAF.DA.DIFAllocator;
import rina.src.DAF.IRM.IPCResourceManager;
import rina.src.DIF.IPCProcess;
import rina.src.EthShimDIF.IPCProcessShim;

import inet.linklayer.contract.IWiredNic;
import inet.networklayer.common.InterfaceTable;

module Host1APShim
{
    parameters:
        @display("i=device/pc;bgb=255,400");
        @networkNode;
        *.interfaceTableModule = default(absPath(".interfaceTable"));

    gates:
        inout medium @loose;

    submodules:
        applicationProcess1: ApplicationProcess {
            @display("p=70,56");
        }

        difAllocator: DIFAllocator {
            @display("p=210,56");
        }

        ipcProcessShim: IPCProcessShim {
            @display("p=132.95334,303.89334");
        }

        ipcProcess1: IPCProcess {
            @display("p=133,241");
        }

        ipcResourceManager: IPCResourceManager {
            @display("p=133,141");
        }

        interfaceTable: InterfaceTable {
            parameters:
                @display("p=69.93,303.89334;is=s");
        }

        eth: <default("EthernetInterface")> like IWiredNic {
            parameters:
                @display("p=132.95334,362.6,row,60;q=txQueue");
        }
    connections allowunconnected:
        ipcProcessShim.northIo <--> ipcProcess1.southIo++;
        ipcProcessShim.ifOut --> eth.upperLayerIn;
        ipcProcessShim.ifIn <-- eth.upperLayerOut;
        eth.phys <--> { @display("m=s"); } <--> medium;
}
