/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Fri Jul 5 09:34:45 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "QGLViewer/qglviewer.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *fileActionOpen;
    QAction *fileActionSave;
    QAction *fileActionSaveAs;
    QAction *fileActionQuit;
    QAction *spheresActionNew;
    QAction *spheresActionDelete;
    QAction *spheresActionGenerate;
    QAction *eqActionBuild;
    QAction *eqActionStartMode;
    QAction *spheresActionStartMode;
    QAction *eqActionAdvance;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QGLViewer *viewer;
    QWidget *verticalLayout_widget;
    QVBoxLayout *verticalLayout;
    QListWidget *sphereListWidget;
    QPushButton *toggleCheckBtn;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuSpheres;
    QMenu *menuEvent_queue;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(787, 684);
        MainWindow->setMinimumSize(QSize(561, 221));
        fileActionOpen = new QAction(MainWindow);
        fileActionOpen->setObjectName(QString::fromUtf8("fileActionOpen"));
        fileActionSave = new QAction(MainWindow);
        fileActionSave->setObjectName(QString::fromUtf8("fileActionSave"));
        fileActionSaveAs = new QAction(MainWindow);
        fileActionSaveAs->setObjectName(QString::fromUtf8("fileActionSaveAs"));
        fileActionQuit = new QAction(MainWindow);
        fileActionQuit->setObjectName(QString::fromUtf8("fileActionQuit"));
        spheresActionNew = new QAction(MainWindow);
        spheresActionNew->setObjectName(QString::fromUtf8("spheresActionNew"));
        spheresActionDelete = new QAction(MainWindow);
        spheresActionDelete->setObjectName(QString::fromUtf8("spheresActionDelete"));
        spheresActionGenerate = new QAction(MainWindow);
        spheresActionGenerate->setObjectName(QString::fromUtf8("spheresActionGenerate"));
        eqActionBuild = new QAction(MainWindow);
        eqActionBuild->setObjectName(QString::fromUtf8("eqActionBuild"));
        eqActionStartMode = new QAction(MainWindow);
        eqActionStartMode->setObjectName(QString::fromUtf8("eqActionStartMode"));
        spheresActionStartMode = new QAction(MainWindow);
        spheresActionStartMode->setObjectName(QString::fromUtf8("spheresActionStartMode"));
        eqActionAdvance = new QAction(MainWindow);
        eqActionAdvance->setObjectName(QString::fromUtf8("eqActionAdvance"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetMaximumSize);
        horizontalLayout->setContentsMargins(2, -1, 2, -1);
        viewer = new QGLViewer(centralWidget);
        viewer->setObjectName(QString::fromUtf8("viewer"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(viewer->sizePolicy().hasHeightForWidth());
        viewer->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(viewer);

        verticalLayout_widget = new QWidget(centralWidget);
        verticalLayout_widget->setObjectName(QString::fromUtf8("verticalLayout_widget"));
        verticalLayout_widget->setMaximumSize(QSize(150, 16777215));
        verticalLayout = new QVBoxLayout(verticalLayout_widget);
        verticalLayout->setSpacing(3);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        sphereListWidget = new QListWidget(verticalLayout_widget);
        sphereListWidget->setObjectName(QString::fromUtf8("sphereListWidget"));
        sphereListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        sphereListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

        verticalLayout->addWidget(sphereListWidget);

        toggleCheckBtn = new QPushButton(verticalLayout_widget);
        toggleCheckBtn->setObjectName(QString::fromUtf8("toggleCheckBtn"));

        verticalLayout->addWidget(toggleCheckBtn);


        horizontalLayout->addWidget(verticalLayout_widget);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 787, 25));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuSpheres = new QMenu(menuBar);
        menuSpheres->setObjectName(QString::fromUtf8("menuSpheres"));
        menuEvent_queue = new QMenu(menuBar);
        menuEvent_queue->setObjectName(QString::fromUtf8("menuEvent_queue"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuSpheres->menuAction());
        menuBar->addAction(menuEvent_queue->menuAction());
        menuFile->addAction(fileActionOpen);
        menuFile->addAction(fileActionSave);
        menuFile->addAction(fileActionSaveAs);
        menuFile->addAction(fileActionQuit);
        menuSpheres->addAction(spheresActionStartMode);
        menuSpheres->addAction(spheresActionNew);
        menuSpheres->addAction(spheresActionGenerate);
        menuSpheres->addAction(spheresActionDelete);
        menuEvent_queue->addAction(eqActionStartMode);
        menuEvent_queue->addAction(eqActionBuild);
        menuEvent_queue->addAction(eqActionAdvance);

        retranslateUi(MainWindow);
        QObject::connect(fileActionOpen, SIGNAL(triggered()), MainWindow, SLOT(openSpheres()));
        QObject::connect(fileActionQuit, SIGNAL(triggered()), MainWindow, SLOT(close()));
        QObject::connect(fileActionSave, SIGNAL(triggered()), MainWindow, SLOT(saveSpheres()));
        QObject::connect(fileActionSaveAs, SIGNAL(triggered()), MainWindow, SLOT(saveSpheresAs()));
        QObject::connect(spheresActionNew, SIGNAL(triggered()), MainWindow, SLOT(addNewSphere()));
        QObject::connect(spheresActionDelete, SIGNAL(triggered()), MainWindow, SLOT(deleteSelectedSpheres()));
        QObject::connect(spheresActionGenerate, SIGNAL(triggered()), MainWindow, SLOT(generateSpheres()));
        QObject::connect(viewer, SIGNAL(drawNeeded()), MainWindow, SLOT(drawViewer()));
        QObject::connect(toggleCheckBtn, SIGNAL(clicked()), MainWindow, SLOT(toggleAllSpheresCheckState()));
        QObject::connect(sphereListWidget, SIGNAL(itemChanged(QListWidgetItem*)), viewer, SLOT(update()));
        QObject::connect(sphereListWidget, SIGNAL(customContextMenuRequested(QPoint)), MainWindow, SLOT(showSpheresMenuAt(QPoint)));
        QObject::connect(eqActionBuild, SIGNAL(triggered()), MainWindow, SLOT(buildEventQueue()));
        QObject::connect(eqActionStartMode, SIGNAL(triggered()), MainWindow, SLOT(enterEventQueueMode()));
        QObject::connect(MainWindow, SIGNAL(modeEntered(Mode)), viewer, SLOT(update()));
        QObject::connect(spheresActionStartMode, SIGNAL(triggered()), MainWindow, SLOT(enterSpheresMode()));
        QObject::connect(eqActionAdvance, SIGNAL(triggered()), MainWindow, SLOT(advanceEventQueue()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Thickness diagram interface", 0, QApplication::UnicodeUTF8));
        fileActionOpen->setText(QApplication::translate("MainWindow", "Open", 0, QApplication::UnicodeUTF8));
        fileActionOpen->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        fileActionSave->setText(QApplication::translate("MainWindow", "Save", 0, QApplication::UnicodeUTF8));
        fileActionSave->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        fileActionSaveAs->setText(QApplication::translate("MainWindow", "Save As", 0, QApplication::UnicodeUTF8));
        fileActionSaveAs->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+S", 0, QApplication::UnicodeUTF8));
        fileActionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        fileActionQuit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        spheresActionNew->setText(QApplication::translate("MainWindow", "New", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        spheresActionNew->setToolTip(QApplication::translate("MainWindow", "New sphere", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        spheresActionDelete->setText(QApplication::translate("MainWindow", "Delete selected", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        spheresActionDelete->setToolTip(QApplication::translate("MainWindow", "Delete selected sphere(s)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        spheresActionGenerate->setText(QApplication::translate("MainWindow", "Generate", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        spheresActionGenerate->setToolTip(QApplication::translate("MainWindow", "Generate spheres", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        spheresActionGenerate->setShortcut(QApplication::translate("MainWindow", "Ctrl+G", 0, QApplication::UnicodeUTF8));
        eqActionBuild->setText(QApplication::translate("MainWindow", "Build", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        eqActionBuild->setToolTip(QApplication::translate("MainWindow", "Build event queue", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        eqActionStartMode->setText(QApplication::translate("MainWindow", "Start mode", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        eqActionStartMode->setToolTip(QApplication::translate("MainWindow", "Start event queue mode", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        spheresActionStartMode->setText(QApplication::translate("MainWindow", "Start mode", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        spheresActionStartMode->setToolTip(QApplication::translate("MainWindow", "Start spheres mode", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        eqActionAdvance->setText(QApplication::translate("MainWindow", "Advance", 0, QApplication::UnicodeUTF8));
        eqActionAdvance->setShortcut(QApplication::translate("MainWindow", "Return", 0, QApplication::UnicodeUTF8));
        toggleCheckBtn->setText(QApplication::translate("MainWindow", "Display/Hide All", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuSpheres->setTitle(QApplication::translate("MainWindow", "Spheres", 0, QApplication::UnicodeUTF8));
        menuEvent_queue->setTitle(QApplication::translate("MainWindow", "Event queue", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
