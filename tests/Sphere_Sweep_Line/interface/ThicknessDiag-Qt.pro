CONFIG *= qt opengl
QT *= core gui opengl xml

TARGET = ThicknessDiag-Qt
TEMPLATE = app

LIBS *= -lQGLViewer -lGLU -lCGAL -lgmp -lmpfr -lboost_thread -lboost_system

SOURCES *= main.cpp\
        mainwindow.cpp \
    sphereformdialog.cpp \
    generatespheresdialog.cpp

HEADERS *= mainwindow.h \
    sphereformdialog.h \
    generatespheresdialog.h

INCLUDEPATH *= ..

FORMS *= mainwindow.ui \
    sphereformdialog.ui \
    generatespheresdialog.ui

QMAKE_CXXFLAGS *= -frounding-math
