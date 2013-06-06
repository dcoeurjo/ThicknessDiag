# Project name
project(LinearCellComplex)

# Build in 'Debug' mode
set(CMAKE_BUILD_TYPE Debug)

# CGAL's policies for CMake
cmake_minimum_required(VERSION 2.6.2)
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

# Packages and their components
# CGAL
find_package(CGAL REQUIRED COMPONENTS)
include(${CGAL_USE_FILE})
# Boost
find_package(Boost REQUIRED)
# Add other packages here

# Entries for target
add_executable(${CMAKE_PROJECT_NAME}
    # Add other sources here
    main.cpp)
add_to_cached_list(CGAL_EXECUTABLE_TARGETS ${CMAKE_PROJECT_NAME})

# Link the executable to CGAL and third-party libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    # Add other libraries here
    ${CGAL_3RD_PARTY_LIBRARIES}
    ${CGAL_LIBRARIES})
