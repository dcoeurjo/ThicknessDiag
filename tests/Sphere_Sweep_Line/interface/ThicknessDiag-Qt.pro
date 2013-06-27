CONFIG *= qt opengl
QT     *= core gui opengl xml

TARGET = ThicknessDiag-Qt
TEMPLATE = app

LIBS *= -lQGLViewer -lGLU -lCGAL -lgmp -lmpfr -lboost_thread -lboost_system

SOURCES *= main.cpp\
        mainwindow.cpp \
    sphereformdialog.cpp

HEADERS *= mainwindow.h \
    sphereformdialog.h

INCLUDEPATH *= ..

FORMS *= mainwindow.ui \
    sphereformdialog.ui

QMAKE_CXXFLAGS *= -frounding-math