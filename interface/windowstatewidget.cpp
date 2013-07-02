#include "windowstatewidget.h"
#include "sphereintersecterproxy.h"

WindowStateWidget::WindowStateWidget(MainWindow *w) :
    QWidget(w)
{
    // Create proxy (useful for accessing si object)
    si_proxy = new SphereIntersecterProxy();
}

WindowStateWidget::~WindowStateWidget()
{ delete si_proxy; }

void WindowStateWidget::setup()
{
    // Setup menu
    QMenu *menu = mw->menuBar()->addMenu(menuTitle());
    setupMenu(menu);
    menu->show();

    // Setup sidebar
    QWidget *sidebar = new QWidget(mw->centralWidget());
    setupSidebar(sidebar);
    sidebar->show();
}