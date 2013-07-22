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
find_package(CGAL QUIET REQUIRED)

# Include helper file
include(${CGAL_USE_FILE})

# Boost and its components
find_package(Boost QUIET COMPONENTS python REQUIRED)
find_package(PythonLibs)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

# Include directories for project
include_directories(${CMAKE_SOURCE_DIR}/..)

# Extra compile flag for template instanciations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-implicit-templates")

# Shared library
add_library(${CMAKE_PROJECT_NAME} SHARED python_export.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME}
    ${Boost_LIBRARIES} ${PYTHON_LIBRARIES}
    ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
