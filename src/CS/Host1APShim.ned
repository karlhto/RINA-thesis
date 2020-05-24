
package rina.src.CS;

import rina.src.DAF.ApplicationProcess;
import rina.src.DAF.DA.DIFAllocator;
import rina.src.DAF.IRM.IPCResourceManager;
import rina.src.DIF.IPCProcess;
import rina.src.EthShimDIF.IPCProcessShim;
import rina.src.EthShimDIF.RINArp.RINArp;

module Host1APShim
{
    parameters:
        @display("i=device/pc;bgb=255,400");
        @node;

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
            @display("p=133,323");
        }

        ipcProcess1: IPCProcess {
            @display("p=133,241");
        }

        ipcResourceManager: IPCResourceManager {
            @display("p=133,141");
        }
    connections allowunconnected:
        ipcProcess1.northIo++ <--> applicationProcess1.southIo++;
        ipcProcessShim.northIo <--> ipcProcess1.southIo++;
}
