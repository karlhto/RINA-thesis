set(LIBRINASIM_SRC
        Addons/Actions/FailureSimulation/FailureSimulation.cc
        Addons/Actions/FlowKiller/FlowKiller.cc
        Addons/Channels/ChangingErrorChannel/ChangerErrorChannel.cc
        Addons/Channels/ChangingErrorChannel/ChangingErrorChannel.cc
        Addons/Channels/EthChannel/EthChannel.cc
        Addons/DataInjectors/FlowsSimulation/Comparator_FWDG/Inj_Comparator.cc
        Addons/DataInjectors/FlowsSimulation/Flows/client_t.cc
        Addons/DataInjectors/FlowsSimulation/Flows/Flow_t.cc
        Addons/DataInjectors/FlowsSimulation/Flows/sender_t.cc
        Addons/DataInjectors/FlowsSimulation/Flows/server_t.cc
        Addons/DataInjectors/FlowsSimulation/Implementations/VDT/VDT_Listener.cc
        Addons/DataInjectors/FlowsSimulation/Implementations/VDT/VDT.cc
        Addons/DataInjectors/FlowsSimulation/PDUandData/PDU_Req.cc
        Addons/DataInjectors/FlowsSimulation/Inj_t.cc
        Addons/DataInjectors/ReachabilityTest/ReachabilityTest_Comparator.cc
        Addons/DataInjectors/ReachabilityTest/ReachabilityTest.cc
        Addons/DataInjectors/ReachabilityTest/ReachabilityTest_Listener.cc
        Common/APNamingInfo.cc
        Common/APN.cc
        Common/CDAPProcessingBase.cc
        Common/Address.cc
        Common/ExternConsts.cc
        Common/DAP.cc
        Common/CongestionDescriptor.cc
        Common/ConnectionId.cc
        Common/Policy.cc
        Common/Flow.cc
        Common/PDU.cc
        Common/QoSCube.cc
        Common/QueueInfo.cc
        Common/QoSReq.cc
        Common/SDU.cc
        Common/RINASignals.cc
        Common/SHA256.cc
        Common/Socket.cc
        Common/Test.cc
        Common/Utils.cc
        DAF/AE/AEMonitor/AEMonitor.cc
        DAF/AE/AEBase.cc
        DAF/AE/AE.cc
        DAF/AE/AEListeners.cc
        DAF/AEManagement/AEMgmt.cc
        DAF/AEManagement/AEMgmtListeners.cc
        DAF/AEManagement/AEMgmtBase.cc
        DAF/AP/APBase.cc
        DAF/AP/AP.cc
        DAF/AP/APIObjBase.cc
        DAF/AP/APIReqObj.cc
        DAF/AP/APIResult.cc
        DAF/AP/APIRetObj.cc
        DAF/AP/APISocketObj.cc
        DAF/AP/APListeners.cc
        DAF/CDAP/AuthListeners.cc
        DAF/CDAP/Auth.cc
        DAF/CDAP/CACEListeners.cc
        DAF/CDAP/CACE.cc
        DAF/CDAP/CDAPListeners.cc
        DAF/CDAP/CDAP.cc
        DAF/CDAP/CDAPMsgLogEntry.cc
        DAF/CDAP/CDAPMsgLog.cc
        DAF/CDAP/CDAPSplitter.cc
        DAF/DA/DA.cc
        DAF/DA/DirectoryEntry.cc
        DAF/DA/Directory.cc
        DAF/DA/NamingInformation.cc
        DAF/DA/NamingInformationEntry.cc
        DAF/DA/NeighborTable.cc
        DAF/DA/NeighborTableEntry.cc
        DAF/DA/SearchTable.cc
        DAF/DA/SearchTableEntry.cc
        DAF/Enrollment/DAFEnrollmentBase.cc
        DAF/Enrollment/CACEBase.cc
        DAF/Enrollment/CACEGeneric.cc
        DAF/Enrollment/CACEMgmt.cc
        DAF/Enrollment/DAFEnrollment.cc
        DAF/Enrollment/DAFEnrollmentListeners.cc
        DAF/Enrollment/DAFEnrollmentNotifierBase.cc
        DAF/Enrollment/DAFEnrollmentNotifier.cc
        DAF/Enrollment/DAFEnrollmentNotifierListeners.cc
        DAF/Enrollment/DAFEnrollmentObj.cc
        DAF/Enrollment/DAFEnrollmentStateTable.cc
        DAF/Enrollment/DAFEnrollmentStateTableEntry.cc
        DAF/Enrollment/DAFOperationObj.cc
        DAF/IRM/IRMListeners.cc
        DAF/IRM/IRM.cc
        DAF/IRM/ConnectionTable.cc
        DAF/IRM/ConnectionTableEntry.cc
        DAF/RIB/DAFRIBdListeners.cc
        DAF/RIB/DAFRIBd.cc
        DAF/RIB/DAFRIBdBase.cc
        DIF/Delimiting/Data.cc
        DIF/Delimiting/Delimiting.cc
        DIF/Delimiting/PDUData.cc
        DIF/Delimiting/UserDataField.cc
        DIF/EFCP/DTCP/DTCPState.cc
        DIF/EFCP/DTCP/DTCP.cc
        DIF/EFCP/DTCP/RXControl.cc
        DIF/EFCP/DTP/DTPState.cc
        DIF/EFCP/DTP/DTP.cc
        DIF/EFCP/DTP/DataTransferPDU.cc
        DIF/EFCP/DTP/DumbGate.cc
        DIF/EFCP/EFCPTable/EFCPTableEntry.cc
        DIF/EFCP/EFCPTable/EFCPTable.cc
        DIF/EFCP/EFCP.cc
        DIF/EFCP/EFCPInstance.cc
        DIF/EFCP/EFCPListeners.cc
        DIF/EFCP/EFCPPolicySet.cc
        DIF/EFCP/MockEFCPI.cc
        DIF/Enrollment/EnrollmentBase.cc
        DIF/Enrollment/Enrollment.cc
        DIF/Enrollment/EnrollmentListeners.cc
        DIF/Enrollment/EnrollmentNotifierBase.cc
        DIF/Enrollment/EnrollmentNotifier.cc
        DIF/Enrollment/EnrollmentNotifierListeners.cc
        DIF/Enrollment/EnrollmentStateTable.cc
        DIF/Enrollment/EnrollmentObj.cc
        DIF/Enrollment/EnrollmentStateTableEntry.cc
        DIF/Enrollment/OperationObj.cc
        DIF/FA/FABase.cc
        DIF/FA/FA.cc
        DIF/FA/FAIBase.cc
        DIF/FA/FAI.cc
        DIF/FA/FANotifierBase.cc
        DIF/FA/FAIListeners.cc
        DIF/FA/FAListeners.cc
        DIF/FA/FANotifier.cc
        DIF/FA/FANotifierListeners.cc
        DIF/FA/NFlowTable.cc
        DIF/FA/NFlowTableEntry.cc
        DIF/NSM/NSM.cc
        DIF/NSM/DirectoryForwardingTable.cc
        DIF/NSM/LocalDirectoryCache.cc
        DIF/RA/PDUFG/IntBbPDUFG.cc
        DIF/RA/PDUFG/IntPDUFG.cc
        DIF/RA/PDUFG/PDUFGNeighbor.cc
        DIF/RA/NM1FlowTableItem.cc
        DIF/RA/NM1FlowTable.cc
        DIF/RA/RABase.cc
        DIF/RA/RA.cc
        DIF/RA/RAListeners.cc
        DIF/RIB/RIBdBase.cc
        DIF/RIB/RIBd.cc
        DIF/RIB/RIBdListeners.cc
        DIF/RIB/RIBdSplitter.cc
        DIF/RMT/RMTBase.cc
        DIF/RMT/RMT.cc
        DIF/RMT/RMTModuleAllocator.cc
        DIF/RMT/RMTListeners.cc
        DIF/RMT/RMTPort.cc
        DIF/RMT/RMTQueue.cc
        DIF/Routing/IntRoutingUpdate.cc
        DIF/Routing/IntBbRouting.cc
        DIF/Routing/IntRouting.cc
        DIF/Routing/RoutingNotifierListeners.cc
        DIF/Routing/RoutingNotifier.cc
        DIF/Routing/RoutingNotifierBase.cc
        ShimDIF/Shim.cc)

set(LIBRINASIM_MESSAGES
        Common/PDU.msg
        Common/SDU.msg
        Common/SDUData.msg
        DAF/CDAP/CDAPMessage.msg
        DIF/Delimiting/Data.msg
        DIF/Delimiting/DelimitingTimers.msg
        DIF/Delimiting/PDUData.msg
        DIF/Delimiting/UserDataField.msg
        DIF/EFCP/DTCP/ControlPDU.msg
        DIF/EFCP/DTCP/DTCPTimers.msg
        DIF/EFCP/DTP/DataTransferPDU.msg
        DIF/EFCP/DTP/DTPTimers.msg
        DIF/EFCP/ManagementPDU.msg)

########################################################################################################################
add_library(librinasimcore OBJECT)
# Need to add .msg to sources to treat them with CLion
target_sources(librinasimcore
        PRIVATE ${LIBRINASIM_SRC} ${LIBRINASIM_MESSAGES})
target_include_directories(librinasimcore
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
               ${PROJECT_SOURCE_DIR}/policies)

# Generate the OMNeT++ message headers and source
generate_opp_message(librinasimcore
        MESSAGE_FILES ${LIBRINASIM_MESSAGES} LEGACY)

target_compile_definitions(librinasimcore PUBLIC
        ${RINASIM_DEFINITIONS})

### Add compile options for the library
target_compile_options(librinasimcore PUBLIC
        # There are a lot of these due to opp_msc not marking things as override
        "-Wno-inconsistent-missing-override"
        # Make sure not to omit any frame pointer for good measure
        "-fno-omit-frame-pointer"
        )

set_target_properties(librinasimcore PROPERTIES
        OUTPUT_NAME rinasimcore
        OMNETPP_LIBRARY ON
        NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

#Add compile features, including language and other extensions
target_compile_features(librinasimcore PUBLIC cxx_std_17)
target_compile_features(librinasimcore PRIVATE cxx_constexpr)

# This pulls in all the requirements for building with OMNet++
# opp_interface is part of the omnetpp-cmake repository
target_link_libraries(librinasimcore PUBLIC opp_envir opp_interface opp_main ${PROJECT_SOURCE_DIR}/libINET.so)

## END

