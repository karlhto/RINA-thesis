package rina.src.EthShimDIF.ShimFA;

module ShimFlowAllocator {
    parameters:
        @display("i=block/fork;bgb=424,357");
    submodules:
        fa: ShimFA {
            @display("p=75,64");
        }
}
