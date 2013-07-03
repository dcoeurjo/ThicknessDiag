#include "window.h"

#include <QStatusBar>

#include "windowstatewidget.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent)
{
    WindowStateWidget *wsw = new WindowStateWidget(siProxyMember);
    setCentralWidget(wsw);
}