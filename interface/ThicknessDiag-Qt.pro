CONFIG *= qt opengl
QT *= core gui opengl xml

TARGET = ThicknessDiag-Qt
TEMPLATE = app

LIBS *= -lQGLViewer -lGLU -lCGAL -lgmp -lmpfr -lboost_thread -lboost_system

SOURCES *= main.cpp\
        mainwindow.cpp \
    sphereformdialog.cpp \
    generatespheresdialog.cpp \
    selectspheredialog.cpp \
    windowstatewidget.cpp \
    spheresstatewidget.cpp \
    sphereview.cpp \
    sphereintersecterproxy.cpp

HEADERS *= mainwindow.h \
    sphereformdialog.h \
    generatespheresdialog.h \
    selectspheredialog.h \
    sphereintersecter.h \
    windowstatewidget.h \
    spheresstatewidget.h \
    sphereview.h \
    sphereintersecterproxy.h

INCLUDEPATH *= ..

FORMS *= mainwindow.ui \
    sphereformdialog.ui \
    generatespheresdialog.ui \
    selectspheredialog.ui

QMAKE_CXXFLAGS *= -frounding-math
