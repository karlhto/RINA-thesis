file(GLOB_RECURSE LIBRINASIM_SRC "*.cc")
file(GLOB_RECURSE LIBRINASIM_MESSAGES "*.msg")

########################################################################################################################
add_library(librinasimcore OBJECT "")
target_sources(librinasimcore
        PRIVATE ${LIBRINASIM_SRC})
target_include_directories(librinasimcore
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
               ${PROJECT_SOURCE_DIR}/policies)

#target_link_libraries(empathic_interface INTERFACE empathic_library)
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

# Generate the OMNeT++ message headers and source
generate_opp_message(librinasimcore
        MESSAGE_FILES ${LIBRINASIM_MESSAGES})

# This pulls in all the requirements for building with OMNet++
# opp_interface is part of the omnetpp-cmake repository
target_link_libraries(librinasimcore PUBLIC opp_envir opp_interface opp_main)

## END
