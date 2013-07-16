project(ThicknessDiag-Qt)

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
find_package(CGAL QUIET COMPONENTS Qt4)

# Include helper file
include( ${CGAL_USE_FILE} )

# Boost and its components
find_package(Boost QUIET REQUIRED system)

# Qt4 and its components
set(QT_USE_QTXML     true)
set(QT_USE_QTMAIN    true)
set(QT_USE_QTSCRIPT  true)
set(QT_USE_QTOPENGL  true)
find_package(Qt4 QUIET REQUIRED)
find_package(QGLViewer QUIET REQUIRED)

# Include directories for Qt
include(${QT_USE_FILE})
include_directories(${QT_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Include directories for project
include_directories(..)

# UI files (Qt Designer files)
qt4_wrap_ui(FORM_SOURCES
    generatespheresdialog.ui
    selectspheredialog.ui
    sphereformdialog.ui)

# Use the Qt MOC preprocessor on classes that derives from QObject
qt4_wrap_cpp(MOC_SOURCES
    eventqueuewindowstate.h
    generatespheresdialog.h
    selectspheredialog.h
    sphereformdialog.h
    sphereintersecterproxy.h
    sphereswindowstate.h
    window.h
    windowstate.h
    windowstatewidget.h)

# QRC files (resources files, that contain icons, at least)

# All sources
add_executable(${CMAKE_PROJECT_NAME}
    customviewer.cpp
    drawabletreewidgetitem.cpp
    eventqueuewindowstate.cpp
    generatespheresdialog.cpp
    main.cpp
    nestreewidgetitem.cpp
    selectspheredialog.cpp
    sphereformdialog.cpp
    sphereintersecterproxy.cpp
    spherelistwidgetitem.cpp
    sphereswindowstate.cpp
    spheretreewidgetitem.cpp
    sphereview.cpp
    window.cpp
    windowstate.cpp
    windowstatefactory.cpp
    windowstatewidget.cpp
    windowstatewithmenu.cpp
    ietreewidgetitem.cpp
    circleview.cpp
    ${MOC_SOURCES} ${FORM_SOURCES})
add_to_cached_list(CGAL_EXECUTABLE_TARGETS ${CMAKE_PROJECT_NAME})

# Link the executable to CGAL and third-party libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    ${QT_LIBRARIES} ${QGLVIEWER_LIBRARIES}
    ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})

