#include "windowstate.h"

#include <QAction>

WindowState::WindowState(WindowStateWidget & wsw):
    QObject(&wsw), siProxy(wsw.siProxy()) {}

WindowState::~WindowState() {}

void WindowState::setStatus(const QString &status)
{ wsw().setStatus(status); }

void WindowState::requestEnter()
{ wsw().changeState(*this); }

void WindowState::enterState()
{ onEnterState();
  emit stateEntered(); }

void WindowState::leaveState()
{ onLeaveState();
  emit stateLeft(); }

QAction* WindowState::makeEnterAction()
{
    QAction *action = new QAction(name(), this);
    QObject::connect(action, SIGNAL(triggered()), this, SLOT(requestEnter()));
    return action;
}