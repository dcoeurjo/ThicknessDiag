CONFIG *= qt opengl
QT *= core gui opengl xml

TARGET = ThicknessDiag-Qt
TEMPLATE = app

LIBS *= -lQGLViewer -lGLU -lCGAL -lgmp -lmpfr -lboost_thread -lboost_system

SOURCES *= main.cpp\
    sphereformdialog.cpp \
    generatespheresdialog.cpp \
    selectspheredialog.cpp \
    sphereintersecterproxy.cpp \
    window.cpp \
    windowstate.cpp \
    sphereswindowstate.cpp \
    windowstatewidget.cpp \
    windowstatefactory.cpp

HEADERS *= \
    sphereformdialog.h \
    generatespheresdialog.h \
    selectspheredialog.h \
    sphereintersecter.h \
    sphereintersecterproxy.h \
    window.h \
    sphereswindowstate.h \
    windowstate.h \
    windowstatewidget.h \
    windowstatefactory.h

INCLUDEPATH *= ..

FORMS *= \
    sphereformdialog.ui \
    generatespheresdialog.ui \
    selectspheredialog.ui

QMAKE_CXXFLAGS *= -frounding-math
