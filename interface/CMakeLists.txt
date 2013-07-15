# Created by the script cgal_create_cmake_script_with_options
# This is the CMake script for compiling a set of CGAL applications.

project( ThicknessDiag-Qt )


cmake_minimum_required(VERSION 2.6.2)
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER 2.6)
  if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_GREATER 2.8.3)
    cmake_policy(VERSION 2.8.4)
  else()
    cmake_policy(VERSION 2.6)
  endif()
endif()

set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
 
if ( COMMAND cmake_policy )

  cmake_policy( SET CMP0003 NEW )  

endif()

# CGAL and its components
find_package( CGAL QUIET COMPONENTS Qt4 )

if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  

endif()

# include helper file
include( ${CGAL_USE_FILE} )


# Boost and its components
find_package( Boost REQUIRED )

if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()  

endif()

# include for local directory
include_directories(..)

# include for local package

# Qt4
set( QT_USE_QTXML     true )
set( QT_USE_QTMAIN    true )
set( QT_USE_QTSCRIPT  true )
set( QT_USE_QTOPENGL  true )

find_package(Qt4)  

if ( NOT QT_FOUND )

  message(STATUS "This project requires the Qt4 library, and will not be compiled.")
  return()  

endif()


# Creating entries for target: ThicknessDiag-Qt
# ############################

if ( CGAL_Qt4_FOUND AND QT_FOUND )

  include( ${QT_USE_FILE} )
  include_directories( ${QT_INCLUDE_DIR} )  

  # UI files (Qt Designer files)
  qt4_wrap_ui( DT_UI_FILES generatespheresdialog.ui )
  qt4_wrap_ui( DT_UI_FILES selectspheredialog.ui )
  qt4_wrap_ui( DT_UI_FILES sphereformdialog.ui )

  # qrc files (resources files, that contain icons, at least)

  # use the Qt MOC preprocessor on classes that derives from QObject
  qt4_generate_moc( eventqueuewindowstate.h eventqueuewindowstate.moc )
  qt4_generate_moc( generatespheresdialog.h generatespheresdialog.moc )
  qt4_generate_moc( selectspheredialog.h selectspheredialog.moc )
  qt4_generate_moc( sphereformdialog.h sphereformdialog.moc )
  qt4_generate_moc( sphereintersecterproxy.h sphereintersecterproxy.moc )
  qt4_generate_moc( sphereswindowstate.h sphereswindowstate.moc )
  qt4_generate_moc( window.h window.moc )
  qt4_generate_moc( windowstate.h windowstate.moc )
  qt4_generate_moc( windowstatewidget.h windowstatewidget.moc )

endif()

add_executable( ThicknessDiag-Qt  customviewer.cpp drawabletreewidgetitem.cpp eventqueuewindowstate.cpp generatespheresdialog.cpp main.cpp nestreewidgetitem.cpp selectspheredialog.cpp sphereformdialog.cpp sphereintersecterproxy.cpp spherelistwidgetitem.cpp sphereswindowstate.cpp spheretreewidgetitem.cpp sphereview.cpp window.cpp windowstate.cpp windowstatefactory.cpp windowstatewidget.cpp windowstatewithmenu.cpp windowstatewidget.moc windowstate.moc window.moc sphereswindowstate.moc sphereintersecterproxy.moc sphereformdialog.moc selectspheredialog.moc generatespheresdialog.moc eventqueuewindowstate.moc  ${DT_UI_FILES} ${DT_RESOURCE_FILES} )

add_to_cached_list( CGAL_EXECUTABLE_TARGETS ThicknessDiag-Qt )

# Link the executable to CGAL and third-party libraries
target_link_libraries(ThicknessDiag-Qt  ${QT_LIBRARIES} ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} )

