
package rina.src.EthShimDIF;

simple ShimFA
{
    parameters:
    	@display("i=block/fork");

    	@signal[FA-AllocateResponseNegative](type=Flow?);
    	@signal[FA-AllocateResponsePositive](type=Flow?);
    	@signal[FA-CreateFlowRequestForward](type=Flow?);
    	@signal[FA-AllocateFinishManagement](type=APNIPair?);
    	@signal[FA-CreateFlowResponseNegative](type=Flow?);
    	@signal[FA-CreateFlowResponsePositive](type=Flow?);

    	//double createRequestTimeout @unit(s) = default(10s);

    gates:

}
