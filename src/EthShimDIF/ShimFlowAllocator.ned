package rina.src.EthShimDIF;

import rina.src.DIF.FA.NFlowTable;

module ShimFlowAllocator {
    parameters:
        @display("i=block/fork;bgb=424,357");
    submodules:
        fa: ShimFA {
            @display("p=75,64");
        }

        nFlowTable: NFlowTable {
            @display("p=167,64");
        }
}
