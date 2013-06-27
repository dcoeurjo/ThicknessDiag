#-------------------------------------------------
#
# Project created by QtCreator 2013-06-27T09:18:02
#
#-------------------------------------------------

CONFIG *= qt opengl
QT     *= core gui opengl xml

TARGET = ThicknessDiag-Qt
TEMPLATE = app

LIBS *= -lQGLViewer -lGLU -lCGAL -lgmp -lmpfr -lboost_thread

SOURCES *= main.cpp\
        mainwindow.cpp \
    sphereformdialog.cpp

HEADERS  *= mainwindow.h \
    sphereformdialog.h

INCLUDEPATH *= ..

FORMS    *= mainwindow.ui \
    sphereformdialog.ui
