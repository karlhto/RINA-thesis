file(GLOB_RECURSE POLICY_SRC "*.cc")
file(GLOB_RECURSE POLICY_MESSAGES "*.msg")

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
target_compile_features(librinasim PRIVATE cxx_constexpr)

# Generate the OMNeT++ message headers and source
generate_opp_message(librinasim
        MESSAGE_FILES ${POLICY_MESSAGES})

# This pulls in all the requirements for building with OMNet++
# opp_interface is part of the omnetpp-cmake repository
target_link_libraries(librinasim PUBLIC opp_envir opp_interface opp_main)

## END
