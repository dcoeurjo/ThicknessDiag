#include "windowstate.h"

WindowState::WindowState(WindowStateWidget & wsw):
    QObject(&wsw), siProxy(wsw.siProxy()) {}

WindowState::~WindowState() {}

void WindowState::setStatus(const QString &status)
{ wsw().setStatus(status); }