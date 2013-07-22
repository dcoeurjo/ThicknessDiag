#include "windowstate.h"
#include <QAction>

WindowState::WindowState(WindowStateWidget & wsw, const QString &n):
    QObject(&wsw), siProxy(wsw.siProxy()), wsw(wsw), nameVal(n)
{
    enterActionMember = new QAction(name(), this);
    QObject::connect(enterActionMember, SIGNAL(triggered()),
                     this, SLOT(requestEnter()));
}

WindowState::~WindowState() {}

void WindowState::enterState()
{ onEnterState();
  emit stateEntered(); }

void WindowState::leaveState()
{ onLeaveState();
  emit stateLeft(); }
