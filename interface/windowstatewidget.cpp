#include "windowstatewidget.h"
#include "sphereintersecterproxy.h"

WindowStateWidget::WindowStateWidget(MainWindow *w) :
    QWidget(w)
{
    menuWidget = new QMenu(w->menubar());
    sidebarWidget = new QWidget(w->sidebar());
}

WindowStateWidget::~WindowStateWidget()
{
}

SphereIntersecter& WindowStateWidget::si() const
{ return *SphereIntersecterProxy::access(); }