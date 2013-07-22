project(ThicknessDiagTests)

cmake_minimum_required(VERSION 2.6.2)
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER 2.6)
    if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_GREATER 2.8.3)
        cmake_policy(VERSION 2.8.4)
    else()
        cmake_policy(VERSION 2.6)
    endif()
endif()
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
if (COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif()

# CGAL and its components
find_package(CGAL QUIET REQUIRED)

# Include helper file
include(${CGAL_USE_FILE})

# Boost and its components
find_package(Boost QUIET COMPONENTS system unit_test_framework REQUIRED)

# Include directories for project
include_directories(${CMAKE_SOURCE_DIR}/..)

# All sources
add_executable(${CMAKE_PROJECT_NAME} main.cpp)
add_to_cached_list(CGAL_EXECUTABLE_TARGETS ${CMAKE_PROJECT_NAME})

# Link the executable to CGAL and third-party libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
