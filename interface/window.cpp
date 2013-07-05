#include "window.h"

#include <QGridLayout>
#include <QMenuBar>
#include <QStatusBar>

#include "windowstatewidget.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent)
{
    // Setup basic UI elements
    centralWidget = new QWidget(this);
    gridLayout = new QGridLayout(centralWidget);
    gridLayout->setContentsMargins(1, 1, 1, 1);
    setCentralWidget(centralWidget);

    // Add window state widget
    WindowStateWidget *wsw = new WindowStateWidget(siProxyMember, this);
    gridLayout->addWidget(wsw);
}