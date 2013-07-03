#include "eventqueuewindowstate.h"

EventQueueWindowState::EventQueueWindowState(WindowStateWidget &wsw):
    WindowState(wsw)
{
    setName(tr("Event Queue"));
}

EventQueueWindowState::~EventQueueWindowState() {}

void EventQueueWindowState::onEnterState()
{
}

void EventQueueWindowState::onLeaveState()
{
}
