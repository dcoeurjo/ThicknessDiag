#include "windowstatefactory.h"

#include <QAction>

#include "allwindowstates.h"

WindowStateFactory::WindowStateFactory(WindowStateWidget &wsw):
    wsw(wsw) {}

WindowState& WindowStateFactory::makeState(WindowStateFactory::StateId id)
{
    // Construct state corresponding to given id or
    // raise an "InvalidStateId" exception
    WindowState *state = 0;
    switch (id)
    {
    case SPHERES:
        state = new SpheresWindowState(wsw);
        break;

    case EVENT_QUEUE:
        state = new EventQueueWindowState(wsw);
        break;

    default:
        throw InvalidStateId();
    }

    // Add state's menu item, connected to enter request
    state->setupWidget(&wsw);
    wsw.addState(*state);

    return *state;
}
