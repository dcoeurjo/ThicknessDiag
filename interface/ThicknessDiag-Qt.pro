CONFIG *= qt opengl
QT *= core gui opengl xml

TARGET = ThicknessDiag-Qt
TEMPLATE = app

LIBS *= -lQGLViewer -lGLU -lCGAL -lgmp -lmpfr -lboost_thread -lboost_system

SOURCES *= main.cpp\
        mainwindow.cpp \
    sphereformdialog.cpp \
    generatespheresdialog.cpp \
    selectspheredialog.cpp

HEADERS *= mainwindow.h \
    sphereformdialog.h \
    generatespheresdialog.h \
    selectspheredialog.h \
    sphereintersecter.h \
    eventqueue.h \
    kernel.h

INCLUDEPATH *= ..

FORMS *= mainwindow.ui \
    sphereformdialog.ui \
    generatespheresdialog.ui \
    selectspheredialog.ui

QMAKE_CXXFLAGS *= -frounding-math
