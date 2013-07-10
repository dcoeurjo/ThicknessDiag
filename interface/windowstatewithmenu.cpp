#include "windowstatewithmenu.h"

#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QMainWindow>

WindowStateWithMenu::WindowStateWithMenu(WindowStateWidget &wsw, const QString &n):
    WindowState(wsw, n) {}

WindowStateWithMenu::~WindowStateWithMenu()
{ delete menu; }

void WindowStateWithMenu::setup()
{
    menu = wsw.mw().menuBar()->addMenu(name());
    menu->setEnabled(false);
}

void WindowStateWithMenu::onEnterState()
{
    menu->setEnabled(true);
    foreach (QAction *action, menu->actions())
    { action->setEnabled(true); }
}

void WindowStateWithMenu::onLeaveState()
{
    menu->setEnabled(false);
    foreach (QAction *action, menu->actions())
    { action->setEnabled(false); }
}

void WindowStateWithMenu::addAction(QAction *action)
{
    menu->addAction(action);
    action->setEnabled(false);
}