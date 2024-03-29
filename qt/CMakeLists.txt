project(ThicknessDiag-Qt)

# Qt4
set(QT_USE_QTXML     true)
set(QT_USE_QTMAIN    true)
set(QT_USE_QTSCRIPT  true)
set(QT_USE_QTOPENGL  true)
find_package(Qt4 QUIET)
if (NOT(QT4_FOUND))
    message(FATAL "Target ${PROJECT_NAME} requires Qt4")
endif()
include(${QT_USE_FILE})
include_directories(${QT_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# QGLViewer
find_package(QGLViewer QUIET)
if (NOT(QGLVIEWER_FOUND))
    message(FATAL "Target ${PROJECT_NAME} requires QGLViewer")
endif()

# OpenGL
find_package(OpenGL QUIET)
if (NOT(OPENGL_FOUND))
    message(FATAL "Target ${PROJECT_NAME} requires OpenGL")
endif()
include_directories(${OPENGL_INCLUDE_DIRS})

# GLUT
find_package(GLUT QUIET)
if (NOT(GLUT_FOUND))
    message(FATAL "Target ${PROJECT_NAME} requires GLUT")
endif()
include_directories(${GLUT_INCLUDE_DIRS})

# UI files (Qt Designer files)
qt4_wrap_ui(FORM_SOURCES
    dialogs/generatespheresdialog.ui
    dialogs/selectspheredialog.ui
    dialogs/sphereformdialog.ui)

# Use the Qt MOC preprocessor on classes that derives from QObject
qt4_wrap_cpp(MOC_SOURCES
    dialogs/generatespheresdialog.h
    dialogs/selectspheredialog.h
    dialogs/sphereformdialog.h
    sphereintersecterproxy.h
    window.h
    windowstates/eventqueuewindowstate.h
    windowstates/sphereswindowstate.h
    windowstates/windowstate.h
    windowstates/windowstatewidget.h)

# All sources
add_executable(${PROJECT_NAME}
    circleview.cpp
    customviewer.cpp
    dialogs/generatespheresdialog.cpp
    dialogs/selectspheredialog.cpp
    dialogs/sphereformdialog.cpp
    main.cpp
    sphereintersecterproxy.cpp
    spherelistwidgetitem.cpp
    sphereview.cpp
    treewidgetitems/bpestreewidgetitem.cpp
    treewidgetitems/cetreewidgetitem.cpp
    treewidgetitems/drawabletreewidgetitem.cpp
    treewidgetitems/ietreewidgetitem.cpp
    treewidgetitems/nestreewidgetitem.cpp
    treewidgetitems/pestreewidgetitem.cpp
    treewidgetitems/spheretreewidgetitem.cpp
    window.cpp
    windowstates/eventqueuewindowstate.cpp
    windowstates/sphereswindowstate.cpp
    windowstates/windowstate.cpp
    windowstates/windowstatefactory.cpp
    windowstates/windowstatewidget.cpp
    windowstates/windowstatewithmenu.cpp
    ${MOC_SOURCES} ${FORM_SOURCES})
add_to_cached_list(CGAL_EXECUTABLE_TARGETS ${PROJECT_NAME})

# Link the executable to CGAL and third-party libraries
target_link_libraries(${PROJECT_NAME}
    ${ThicknessDiag_LIBRARIES}
    ${QT_LIBRARIES} ${QGLVIEWER_LIBRARIES}
    ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
