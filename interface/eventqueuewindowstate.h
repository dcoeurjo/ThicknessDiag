#ifndef EVENTQUEUEWINDOWSTATE_H
#define EVENTQUEUEWINDOWSTATE_H

#include "windowstate.h"

class EventQueueWindowState : public WindowState
{
public:
    explicit EventQueueWindowState(WindowStateWidget &wsw);
    ~EventQueueWindowState();

protected:
    void onEnterState();
    void onLeaveState();
};

#endif // EVENTQUEUEWINDOWSTATE_H
