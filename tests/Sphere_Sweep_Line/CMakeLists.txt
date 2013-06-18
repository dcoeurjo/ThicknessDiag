# Created by the script cgal_create_cmake_script_with_options
# This is the CMake script for compiling a set of CGAL applications.
#
# Edited by Jean-Marie Comets, mainly for build flags
cmake_minimum_required(VERSION 2.6.2)
project(Sphere_Sweep_Line)

# default build type
set(DEFAULT_BUILD_TYPE Debug)
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE})
endif()

# CGAL policy for CMake
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER 2.6)
    if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_GREATER 2.8.3)
        cmake_policy(VERSION 2.8.4)
    else()
        cmake_policy(VERSION 2.6)
    endif()
endif()
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif()

# Needed packages
find_package(CGAL COMPONENTS)
find_package(Boost REQUIRED)
# helper file
include(${CGAL_USE_FILE})

# include for local directory
include_directories(${CMAKE_SOURCE_DIR})

# sources
set(PROJECT_SOURCES main.cpp)

# extra options
set(WITH_INSERT_TIMING_DESCRIPTION "Set this option to time each call of the insertion of a sphere")
option(WITH_INSERT_TIMING ${WITH_INSERT_TIMING_DESCRIPTION} FALSE)
if(WITH_INSERT_TIMING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTIME_INSERT")
endif()

set(WITH_GEOMVIEW_DESCRIPTION "Set this option to display output of insertion on Geomview and prompt before passing")
option(WITH_GEOMVIEW_DISPLAY ${WITH_GEOMVIEW_DESCRIPTION} FALSE)
if(WITH_GEOMVIEW_DISPLAY)
    if(WITH_INSERT_TIMING)
        message(FATAL "Cannot use the WITH_INSERT_TIMING and WITH_GEOMVIEW_DISPLAY at the same time (conflict")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDISPLAY_ON_GEOMVIEW")
        list(APPEND PROJECT_SOURCES Geomview_stream_extension.cpp)
    endif()
endif()

# link the executable to CGAL and third-party libraries
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
add_to_cached_list(CGAL_EXECUTABLE_TARGETS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}
    ${CGAL_LIBRARIES}
    ${CGAL_3RD_PARTY_LIBRARIES}
    )
