#include "window.h"

#include <QStatusBar>

#include "windowstatewidget.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent)
{
    WindowStateWidget *wsw = new WindowStateWidget(this);
    setCentralWidget(wsw);
}

void Window::showStatus(const QString &status)
{ statusBar()->showMessage(status, 3000); }