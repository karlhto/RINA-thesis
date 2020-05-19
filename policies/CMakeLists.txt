set(POLICY_SRC
        DIF/EFCP/DTCP/ECN/ECNPolicyDefault/ECNPolicyDefault.cc
        DIF/EFCP/DTCP/ECN/ECNPolicyBase.cc
        DIF/EFCP/DTCP/ECNSlowDown/DTCPECNSlowDownPolicyDefault/DTCPECNSlowDownPolicyDefault.cc
        DIF/EFCP/DTCP/ECNSlowDown/DTCPECNSlowDownPolicyTCP/DTCPECNSlowDownPolicyTCP.cc
        DIF/EFCP/DTCP/ECNSlowDown/ECNSlowDownPolicyDefault/ECNSlowDownPolicyDefault.cc
        DIF/EFCP/DTCP/ECNSlowDown/ECNSlowDownPolicyBase.cc
        DIF/EFCP/DTCP/LostControlPDU/LostControlPDUPolicyDefault/LostControlPDUPolicyDefault.cc
        DIF/EFCP/DTCP/LostControlPDU/LostControlPDUPolicyBase.cc
        DIF/EFCP/DTCP/NoOverridePeak/NoOverridePeakPolicyDefault/NoOverridePeakPolicyDefault.cc
        DIF/EFCP/DTCP/NoOverridePeak/NoOverridePeakPolicyBase.cc
        DIF/EFCP/DTCP/NoRateSlowDown/NoRateSlowDownPolicyDefault/NoRateSlowDownPolicyDefault.cc
        DIF/EFCP/DTCP/NoRateSlowDown/NoRateSlowDownPolicyBase.cc
        DIF/EFCP/DTCP/RateReduction/RateReductionPolicyDefault/RateReductionPolicyDefault.cc
        DIF/EFCP/DTCP/RateReduction/RateReductionPolicyBase.cc
        DIF/EFCP/DTCP/RcvFCOverrun/RcvFCOverrunPolicyDefault/RcvFCOverrunPolicyDefault.cc
        DIF/EFCP/DTCP/RcvFCOverrun/RcvFCOverrunPolicyBase.cc
        DIF/EFCP/DTCP/RcvrAck/RcvrAckPolicyDefault/RcvrAckPolicyDefault.cc
        DIF/EFCP/DTCP/RcvrAck/RcvrAckPolicyLG/RcvrAckPolicyLG.cc
        DIF/EFCP/DTCP/RcvrAck/RcvrAckPolicyBase.cc
        DIF/EFCP/DTCP/RcvrControlAck/RcvrControlAckPolicyDefault/RcvrControlAckPolicyDefault.cc
        DIF/EFCP/DTCP/RcvrControlAck/RcvrControlAckPolicyBase.cc
        DIF/EFCP/DTCP/RcvrFC/RcvrFCPolicyDefault/RcvrFCPolicyDefault.cc
        DIF/EFCP/DTCP/RcvrFC/RcvrFCPolicyDefaultWithCounters/RcvrFCPolicyDefaultWithCounters.cc
        DIF/EFCP/DTCP/RcvrFC/RcvrFCPolicyBase.cc
        DIF/EFCP/DTCP/ReceivingFC/ReceivingFCPolicyDefault/ReceivingFCPolicyDefault.cc
        DIF/EFCP/DTCP/ReceivingFC/ReceivingFCPolicyBase.cc
        DIF/EFCP/DTCP/ReconcileFC/ReconcileFCPolicyDefault/ReconcileFCPolicyDefault.cc
        DIF/EFCP/DTCP/ReconcileFC/ReconcileFCPolicyBase.cc
        DIF/EFCP/DTCP/RxTimerExpiry/RxTimerExpiryPolicyDefault/RxTimerExpiryPolicyDefault.cc
        DIF/EFCP/DTCP/RxTimerExpiry/RxTimerExpiryPolicyBase.cc
        DIF/EFCP/DTCP/SenderAck/SenderAckPolicyDefault/SenderAckPolicyDefault.cc
        DIF/EFCP/DTCP/SenderAck/SenderAckPolicyLG/SenderAckPolicyLG.cc
        DIF/EFCP/DTCP/SenderAck/SenderAckPolicyTCP/SenderAckPolicyTCP.cc
        DIF/EFCP/DTCP/SenderAck/SenderAckPolicyBase.cc
        DIF/EFCP/DTCP/SendingAck/SendingAckPolicyDefault/SendingAckPolicyDefault.cc
        DIF/EFCP/DTCP/SendingAck/SendingAckPolicyBase.cc
        DIF/EFCP/DTCP/SndFCOverrun/SndFCOverrunPolicyDefault/SndFCOverrunPolicyDefault.cc
        DIF/EFCP/DTCP/SndFCOverrun/SndFCOverrunPolicyBase.cc
        DIF/EFCP/DTCP/TxControl/TxControlPolicyDefault/TxControlPolicyDefault.cc
        DIF/EFCP/DTCP/TxControl/TxControlPolicyLG/TxControlPolicyLG.cc
        DIF/EFCP/DTCP/TxControl/TxControlPolicyTCPTahoe/TxControlPolicyTCPTahoe.cc
        DIF/EFCP/DTCP/TxControl/TxControlPolicyBase.cc
        DIF/EFCP/DTP/InitialSeqNum/InitialSeqNumPolicyDefault/InitialSeqNumPolicyDefault.cc
        DIF/EFCP/DTP/InitialSeqNum/InitialSeqNumPolicyBase.cc
        DIF/EFCP/DTP/RTTEstimator/RTTEstimatorPolicyDefault/RTTEstimatorPolicyDefault.cc
        DIF/EFCP/DTP/RTTEstimator/RTTEstimatorPolicyLG/RTTEstimatorPolicyLG.cc
        DIF/EFCP/DTP/RTTEstimator/RTTEstimatorPolicyTCP/RTTEstimatorPolicyTCP.cc
        DIF/EFCP/DTP/RTTEstimator/RTTEstimatorPolicyBase.cc
        DIF/EFCP/DTP/RcvrInactivity/RcvrInactivityPolicyDefault/RcvrInactivityPolicyDefault.cc
        DIF/EFCP/DTP/RcvrInactivity/RcvrInactivityPolicyBase.cc
        DIF/EFCP/DTP/SenderInactivity/SenderInactivityPolicyDefault/SenderInactivityPolicyDefault.cc
        DIF/EFCP/DTP/SenderInactivity/SenderInactivityPolicyBase.cc
        DIF/EFCP/EFCPPolicy.cc
        DIF/FA/AllocateRetry/LimitedRetries/LimitedRetries.cc
        DIF/FA/AllocateRetry/NoRetry/NoRetry.cc
        DIF/FA/AllocateRetry/UnlimitedRetries/UnlimitedRetries.cc
        DIF/FA/MultilevelQoS/QoSIdComparer/QoSIdComparer.cc
        DIF/FA/MultilevelQoS/QoSMinComparer/QoSMinComparer.cc
        DIF/FA/NewFlowRequest/MinComparer/MinComparer.cc
        DIF/FA/NewFlowRequest/ScoreComparer/ScoreComparer.cc
        DIF/FA/NewFlowRequest/NewFlowRequestBase.cc
        DIF/RA/AddressComparator/EndPoint/EndPoint.cc
        DIF/RA/AddressComparator/ExactMatch/ExactMatch.cc
        DIF/RA/AddressComparator/PrefixMatch/PrefixMatch.cc
        DIF/RA/AddressComparator/AddressComparatorBase.cc
        DIF/RA/PDUFG/BiDomainGenerator/BiDomainGenerator.cc
        DIF/RA/PDUFG/GRE/Centralized/GRE_ClosFabric.cc
        DIF/RA/PDUFG/GRE/Centralized/GRE_ClosSpine.cc
        DIF/RA/PDUFG/GRE/Centralized/GRE_ClosToR.cc
        DIF/RA/PDUFG/GRE/Centralized/GRE_ManagerClos.cc
        DIF/RA/PDUFG/GRE/ClosRouting/GRE_Clos0R.cc
        DIF/RA/PDUFG/GRE/ClosRouting/GRE_Clos1R.cc
        DIF/RA/PDUFG/GRE/ClosRouting/GRE_Clos2R.cc
        DIF/RA/PDUFG/GRE/ClosRouting/GRE_ClosR.cc
        DIF/RA/PDUFG/GRE/ClosStatic/GRE_Clos0S.cc
        DIF/RA/PDUFG/GRE/ClosStatic/GRE_Clos1S.cc
        DIF/RA/PDUFG/GRE/ClosStatic/GRE_Clos2S.cc
        DIF/RA/PDUFG/GRE/Simple/GRE_Simple.cc
        DIF/RA/PDUFG/HierarchicalGenerator/HierarchicalGenerator.cc
        DIF/RA/PDUFG/HopsQoSMEntries/HopsQoSMEntries.cc
        DIF/RA/PDUFG/HopsSingle1Entry/HopsSingle1Entry.cc
        DIF/RA/PDUFG/HopsSingleMEntries/HopsSingleMEntries.cc
        DIF/RA/PDUFG/IPLSPDUFG/IPLSPDUFG.cc
        DIF/RA/PDUFG/IQoSAwareMEntries/IQoSAwareMEntries.cc
        DIF/RA/PDUFG/LatGenerator/LatGenerator.cc
        DIF/RA/PDUFG/LatencySingle1Entry/LatencySingle1Entry.cc
        DIF/RA/PDUFG/LatencySingleMEntries/LatencySingleMEntries.cc
        DIF/RA/PDUFG/MSimpleGenerator/MSimpleGenerator.cc
        DIF/RA/PDUFG/NeiGen/NeiGen.cc
        DIF/RA/PDUFG/OFStaticGenerator/OFStaticGenerator.cc
        DIF/RA/PDUFG/PLQoSAwareMEntries/PLQoSAwareMEntries.cc
        DIF/RA/PDUFG/PSTLatOrHopMEntries/PSTLatOrHopMEntries.cc
        DIF/RA/PDUFG/PortsLoadGenerator/PortsLoadGenerator.cc
        DIF/RA/PDUFG/QoSAwareMEntries/QoSAwareMEntries.cc
        DIF/RA/PDUFG/QoSDomainGenerator/QoSDomainGenerator.cc
        DIF/RA/PDUFG/SimpleDCGenerator/SimpleDCGenerator.cc
        DIF/RA/PDUFG/SimpleGenerator/SimpleGenerator.cc
        DIF/RA/PDUFG/SimpleHopMEntries/SimpleHopMEntries.cc
        DIF/RA/PDUFG/SimpleLatOrHopMEntries/SimpleLatOrHopMEntries.cc
        DIF/RA/PDUFG/SingleDomainGenerator/SingleDomainGenerator.cc
        DIF/RA/PDUFG/StatGen/StatGen.cc
        DIF/RA/PDUFG/StaticGenerator/StaticGenerator.cc
        DIF/RA/PDUFG/VoidDCGenerator/VoidDCGenerator.cc
        DIF/RA/QueueAlloc/QueuePerNCU/QueuePerNCU.cc
        DIF/RA/QueueAlloc/QueuePerNFlow/QueuePerNFlow.cc
        DIF/RA/QueueAlloc/QueuePerNQoS/QueuePerNQoS.cc
        DIF/RA/QueueAlloc/QueuePerNQoSxPLen/QueuePerNQoSxPLen.cc
        DIF/RA/QueueAlloc/SingleQueue/SingleQueue.cc
        DIF/RA/QueueAlloc/QueueAllocBase.cc
        DIF/RA/QueueIDGen/IDPerNCU/IDPerNCU.cc
        DIF/RA/QueueIDGen/IDPerNFlow/IDPerNFlow.cc
        DIF/RA/QueueIDGen/IDPerNQoS/IDPerNQoS.cc
        DIF/RA/QueueIDGen/IDPerNQoSxPLen/IDPerNQoSxPLen.cc
        DIF/RA/QueueIDGen/SingleID/SingleID.cc
        DIF/RA/QueueIDGen/QueueIDGenBase.cc
        DIF/RMT/MaxQueue/BLUEDropper/BLUEDropper.cc
        DIF/RMT/MaxQueue/DumbMaxQ/DumbMaxQ.cc
        DIF/RMT/MaxQueue/ECNMarker/ECNMarker.cc
        DIF/RMT/MaxQueue/ECNMarkerLG/ECNMarkerLG.cc
        DIF/RMT/MaxQueue/REDDropper/REDDropper.cc
        DIF/RMT/MaxQueue/REDUpstreamNotifier/REDUpstreamNotifier.cc
        DIF/RMT/MaxQueue/ReadRateReducer/ReadRateReducer.cc
        DIF/RMT/MaxQueue/TailDrop/TailDrop.cc
        DIF/RMT/MaxQueue/UpstreamNotifier/UpstreamNotifier.cc
        DIF/RMT/MaxQueue/RMTMaxQBase.cc
        DIF/RMT/Monitor/BEMonitor/BEMonitor.cc
        DIF/RMT/Monitor/DLMonitor/DLMonitor.cc
        DIF/RMT/Monitor/DQMonitor/DQMonitor.cc
        DIF/RMT/Monitor/DQMonitor/dlCUInfo.cc
        DIF/RMT/Monitor/DummyMonitor/DummyMonitor.cc
        DIF/RMT/Monitor/IterativeMonitor/IterativeMonitor.cc
        DIF/RMT/Monitor/IterativeStopMonitor/IterativeStopMonitor.cc
        DIF/RMT/Monitor/ModularMonitor/MM_DL_Drop/MM_DL_Drop.cc
        DIF/RMT/Monitor/ModularMonitor/MM_DL_Out/MM_DL_Out.cc
        DIF/RMT/Monitor/ModularMonitor/MM_DQ_Drop/MM_DQ_Drop.cc
        DIF/RMT/Monitor/ModularMonitor/MM_DQ_Out/MM_DQ_Out.cc
        DIF/RMT/Monitor/ModularMonitor/MM_PDQ_Drop/MM_PDQ_Drop.cc
        DIF/RMT/Monitor/ModularMonitor/MM_P_Out/MM_P_Out.cc
        DIF/RMT/Monitor/ModularMonitor/MM_SPort_Drop/MM_SPort_Drop.cc
        DIF/RMT/Monitor/ModularMonitor/MM_Simple_Drop/MM_Simple_Drop.cc
        DIF/RMT/Monitor/ModularMonitor/MM_Simple_Out/MM_Simple_Out.cc
        DIF/RMT/Monitor/ModularMonitor/MM_WFQ_Drop/MM_WFQ_Drop.cc
        DIF/RMT/Monitor/ModularMonitor/MM_WFQ_Out/MM_WFQ_Out.cc
        DIF/RMT/Monitor/ModularMonitor/MM_eDL_Drop/MM_eDL_Drop.cc
        DIF/RMT/Monitor/ModularMonitor/MM_eDL_Out/MM_eDL_Out.cc
        DIF/RMT/Monitor/ModularMonitor/MM_maxDelayLimited_Out/MM_maxDelayLimited_Out.cc
        DIF/RMT/Monitor/ModularMonitor/MM_maxDelay_Out/MM_maxDelay_Out.cc
        DIF/RMT/Monitor/ModularMonitor/MM_maxPST_Out/MM_maxPST_Out.cc
        DIF/RMT/Monitor/ModularMonitor/MM_maxWP_Out/MM_maxWP_Out.cc
        DIF/RMT/Monitor/ModularMonitor/MM_psDQ_Out/MM_psDQ_Out.cc
        DIF/RMT/Monitor/ModularMonitor/SM_Burst/SM_Burst.cc
        DIF/RMT/Monitor/ModularMonitor/ModularMonitor.cc
        DIF/RMT/Monitor/PortsLoadMonitor/PortsLoadMonitor.cc
        DIF/RMT/Monitor/QTAMux/Mux0/Mux0.cc
        DIF/RMT/Monitor/QTAMux/Mux1/Mux1.cc
        DIF/RMT/Monitor/QTAMux/Mux2/Mux2.cc
        DIF/RMT/Monitor/QTAMux/Mux3/Mux3.cc
        DIF/RMT/Monitor/QTAMux/Mux4/Mux4.cc
        DIF/RMT/Monitor/QTAMux/Mux5/Mux5.cc
        DIF/RMT/Monitor/QTAMux/PS0/PS0.cc
        DIF/RMT/Monitor/QTAMux/PS1/PS1.cc
        DIF/RMT/Monitor/QTAMux/PS2/PS2.cc
        DIF/RMT/Monitor/QTAMux/PS3/PS3.cc
        DIF/RMT/Monitor/QTAMux/PS4/PS4.cc
        DIF/RMT/Monitor/QTAMux/QTAMonitor.cc
        DIF/RMT/Monitor/QTAMux/Mux.cc
        DIF/RMT/Monitor/QTAMux/PS.cc
        DIF/RMT/Monitor/REDMonitor/REDMonitor.cc
        DIF/RMT/Monitor/TKMonitor/TKMonitor.cc
        DIF/RMT/Monitor/WeightedFairQMonitor/WeightedFairQMonitor.cc
        DIF/RMT/Monitor/WeightedFairQMonitor/FlowParams.cc
        DIF/RMT/Monitor/eDLMonitor/eDLMonitor.cc
        DIF/RMT/Monitor/RMTQMonitorBase.cc
        DIF/RMT/PDUForwarding/DefaultGW/DefaultGW.cc
        DIF/RMT/PDUForwarding/DomainTable/DomainTable.cc
        DIF/RMT/PDUForwarding/FloodMiniTable/FloodMiniTable.cc
        DIF/RMT/PDUForwarding/GREFWD/Clos/Clos.cc
        DIF/RMT/PDUForwarding/GREFWD/GREFWD.cc
        DIF/RMT/PDUForwarding/GREFWD/GRE_Helpers.cc
        DIF/RMT/PDUForwarding/HierarchicalTable/HierarchicalTable.cc
        DIF/RMT/PDUForwarding/IMultiQoSTable/IMultiQoSTable.cc
        DIF/RMT/PDUForwarding/MiniTable/MiniTable.cc
        DIF/RMT/PDUForwarding/MultiMiniTable/MultiMiniTable.cc
        DIF/RMT/PDUForwarding/MultiQoSTable/MultiQoSTable.cc
        DIF/RMT/PDUForwarding/NeiTable/NeiTable.cc
        DIF/RMT/PDUForwarding/Q2FwdT/Q2FwdT.cc
        DIF/RMT/PDUForwarding/QoSTable/QoSTable.cc
        DIF/RMT/PDUForwarding/SimpleDCForwarding/Edge/SimpleEdgeForwarding.cc
        DIF/RMT/PDUForwarding/SimpleDCForwarding/Fabric/SimpleFabricForwarding.cc
        DIF/RMT/PDUForwarding/SimpleDCForwarding/Spine/SimpleSpineForwarding.cc
        DIF/RMT/PDUForwarding/SimpleDCForwarding/TOR/SimpleTORForwarding.cc
        DIF/RMT/PDUForwarding/SimpleDCForwarding/DCAddr.cc
        DIF/RMT/PDUForwarding/SimpleDCForwarding/SimpleDCForwarding.cc
        DIF/RMT/PDUForwarding/SimpleTable/SimpleTable.cc
        DIF/RMT/PDUForwarding/IntPDUForwarding.cc
        DIF/RMT/Scheduler/DQSch/DQSch.cc
        DIF/RMT/Scheduler/DumbSch/DumbSch.cc
        DIF/RMT/Scheduler/IterativeScheduling/IterativeScheduling.cc
        DIF/RMT/Scheduler/LongestQFirst/LongestQFirst.cc
        DIF/RMT/Scheduler/QTASch/QTASch.cc
        DIF/RMT/Scheduler/WeightedFairQ/WeightedFairQ.cc
        DIF/RMT/Scheduler/RMTSchedulingBase.cc
        DIF/Routing/CentralRouting/RoutingClient.cc
        DIF/Routing/CentralRouting/RoutingDumb.cc
        DIF/Routing/CentralRouting/RoutingManager.cc
        DIF/Routing/CentralRouting/RoutingModule.cc
        DIF/Routing/DCRouting/Fabric_Routing.cc
        DIF/Routing/DCRouting/Edge_Routing.cc
        DIF/Routing/DCRouting/IntDCRouting.cc
        DIF/Routing/DCRouting/Spine_Routing.cc
        DIF/Routing/DCRouting/TOR_Routing.cc
        DIF/Routing/DomainRouting/DV/DV.cc
        DIF/Routing/DomainRouting/LS/LS.cc
        DIF/Routing/DomainRouting/Routing.cc
        DIF/Routing/DomainRouting/rModule.cc
        DIF/Routing/DummyRouting/DummyRouting.cc
        DIF/Routing/PortsLoadRouting/PortsLoadLS/PortsLoadLS.cc
        DIF/Routing/PortsLoadRouting/PortsLoadLS/PortsLoadRU.cc
        DIF/Routing/SimpleRouting/SimpleDV/SimpleDV.cc
        DIF/Routing/SimpleRouting/SimpleLS/SimpleLS.cc
        DIF/Routing/TDomainRouting/Types_TDomainRouting.cc
        DIF/Routing/TSimpleRouting/TSimpleDV/Types_TSimpleDV.cc
        DIF/Routing/TSimpleRouting/TSimpleLS/Types_TSimpleLS.cc
        DIF/Routing/eRouting/eRouting.cc
        DIF/Routing/eRouting/eLink.cc
        DIF/SDUProtection/DummyProtection/DummyProtection.cc
        DIF/SDUProtection/FixedDelay/FixedDelay.cc
        DIF/SDUProtection/IntSDUProtection.cc)

########################################################################################################################
add_library(librinasim SHARED $<TARGET_OBJECTS:librinasimcore>)
target_sources(librinasim
        PRIVATE ${POLICY_SRC})
target_include_directories(librinasim
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src)

#target_link_libraries(empathic_interface INTERFACE empathic_library)
target_compile_definitions(librinasim
        PUBLIC ${RINASIM_DEFINITIONS})

### Add compile options for the library
target_compile_options(librinasim PUBLIC
        # There are a lot of these due to opp_msc not marking things as override
        "-Wno-inconsistent-missing-override"
        # Make sure not to omit any frame pointer for good measure
        "-fno-omit-frame-pointer"
        # Disabled because the things that actually broke it are fixed
        #"-march=native -O2 -pipe -fno-plt"
        )

set_target_properties(librinasim PROPERTIES
        OUTPUT_NAME rinasim
        OMNETPP_LIBRARY ON
        NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

#Add compile features, including language and other extensions
target_compile_features(librinasim PUBLIC cxx_std_17)

# This pulls in all the requirements for building with OMNet++
# opp_interface is part of the omnetpp-cmake repository
target_link_libraries(librinasim PUBLIC opp_envir opp_interface opp_main)

## END

