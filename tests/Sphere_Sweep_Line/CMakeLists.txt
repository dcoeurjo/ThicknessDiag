# Created by the script cgal_create_cmake_script_with_options
# This is the CMake script for compiling a set of CGAL applications.
#
# Edited by Jean-Marie Comets, mainly for build flags
cmake_minimum_required(VERSION 2.6.2)
project(Sphere_Sweep_Line)

# default build type
set(DEFAULT_BUILD_TYPE Debug)
if(CMAKE_BUILD_TYPE STREQUAL "")
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
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
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif()

# Needed packages
find_package(CGAL COMPONENTS)
find_package(Boost REQUIRED)
# helper file
include(${CGAL_USE_FILE})
message(STATUS "use file = ${CGAL_USE_FILE}")

# include for local directory
include_directories(${CMAKE_SOURCE_DIR})

# sources
add_executable(${PROJECT_NAME}
    Geomview_stream_extension.cpp
    main.cpp
    )
add_to_cached_list(CGAL_EXECUTABLE_TARGETS ${PROJECT_NAME})

# link the executable to CGAL and third-party libraries
target_link_libraries(${PROJECT_NAME}
    ${CGAL_LIBRARIES}
    ${CGAL_3RD_PARTY_LIBRARIES}
    )
