project(ThicknessDiag-Geomview)

# Include local directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Sources
set(PROJECT_SOURCES main.cpp)

# Extra options
#
# Geomview display
set(WITH_GEOMVIEW_DESCRIPTION "Set this option to display output of insertion on Geomview and prompt before passing")
set(GEOMVIEW_DISPLAY_FLAG "DISPLAY_ON_GEOMVIEW")
option(WITH_GEOMVIEW_DISPLAY ${WITH_GEOMVIEW_DESCRIPTION} FALSE)
if(WITH_GEOMVIEW_DISPLAY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${GEOMVIEW_DISPLAY_FLAG}")
    list(APPEND PROJECT_SOURCES Geomview_stream_extension.cpp)
endif()
# C++0x mode
set(WITH_CPP0X_MODE_DESCRIPTION "Set this option to compile in C++0x mode")
option(WITH_CPP0X_MODE ${WITH_CPP0X_MODE_DESCRIPTION} FALSE)
if(WITH_CPP0X_MODE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

# Executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
add_to_cached_list(CGAL_EXECUTABLE_TARGETS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}
    ${ThicknessDiag_LIBRARIES})
