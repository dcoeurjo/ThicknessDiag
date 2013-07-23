project(ThicknessDiag-Python)

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
find_package(CGAL REQUIRED)

# Include helper file
include(${CGAL_USE_FILE})

# Boost and its components
find_package(Boost REQUIRED COMPONENTS python)
find_package(PythonLibs 2.7 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

# Include directories for project
include_directories(${CMAKE_SOURCE_DIR}/..)

# Setup library configuration
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-implicit-templates")
set(ThicknessDiag-Python "thickness_diag")
add_library(${ThicknessDiag-Python} SHARED python_export.cpp)
set_target_properties(${ThicknessDiag-Python} PROPERTIES PREFIX "")
target_link_libraries(${ThicknessDiag-Python}
    ${PYTHON_LIBRARIES} ${Boost_LIBRARIES}
    ${CGAL_3RD_PARTY_LIBRARIES} ${CGAL_LIBRARIES})
