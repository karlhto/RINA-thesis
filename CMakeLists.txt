# https://cliutils.gitlab.io/modern-cmake/chapters/basics.html
cmake_minimum_required(VERSION 3.17)

# Convert relative paths to absolute: https://cmake.org/cmake/help/latest/policy/CMP0076.html
if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()

# Project details!
project(RINASim
        VERSION 1.1
        DESCRIPTION "A discrete simulation framework for the clean-slate internet architecture known as RINA"
        LANGUAGES CXX)

# Global compiler choice that SHOULD be solved somehow else?
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

# Whether to enable testing or not
#enable_testing()

# Use libc++ for better debugging capabilities
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-3")

# Define our module path for omnetpp-cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
# Find our OMNet++ 6 installation and import our module in "cmake/"
find_package(OmnetPP 6 REQUIRED)
include(OMNet)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Used in compile definitions
set(RINASIM_DEFINITIONS
        AUTOIMPORT_OMNETPP_NAMESPACE)

add_subdirectory(policies)
add_subdirectory(src)
