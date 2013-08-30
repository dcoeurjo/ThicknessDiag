project(ThicknessDiag)

# Setup CMake policy for CGAL
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

# CGAL
find_package(CGAL)
include(${CGAL_USE_FILE})

# Boost
find_package(Boost REQUIRED system)

# Include project headers
include_directories(${CMAKE_SOURCE_DIR})

# Add project library
set(ThicknessDiag_LIB ${PROJECT_NAME}-lib)
set(ThicknessDiag_LIBRARIES ${ThicknessDiag_LIB}
    ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
add_subdirectory("lib")

# Final executable
set(ThicknessDiag_EXE ${PROJECT_NAME})
add_executable(${ThicknessDiag_EXE} main.cpp)
target_link_libraries(${ThicknessDiag_EXE} ${ThicknessDiag_LIBRARIES})

# C++0x/11 mode
set(WITH_CPP0X_MODE_DESCRIPTION "Set this option to compile in C++0x mode")
option(WITH_CPP0X_MODE ${WITH_CPP0X_MODE_DESCRIPTION} FALSE)
if(WITH_CPP0X_MODE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

# Qt interface extension
set(WITH_QT_DESCRIPTION "Set this option to display output of insertion on Geomview and prompt before passing")
set(QT_DISPLAY_FLAG "DISPLAY_ON_QT")
option(WITH_QT_DISPLAY ${WITH_QT_DESCRIPTION} FALSE)
if(WITH_QT_DISPLAY)
    add_subdirectory("qt")
endif()
