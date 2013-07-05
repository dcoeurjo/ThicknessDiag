#ifndef EVENTQUEUEWINDOWSTATE_H
#define EVENTQUEUEWINDOWSTATE_H

#include "windowstate.h"
#include "eventqueue.h"

class QListWidget;

class EventQueueWindowState : public WindowState
{
public:
    explicit EventQueueWindowState(WindowStateWidget &wsw);
    ~EventQueueWindowState();

    void setup();

protected:
    void onEnterState();
    void onLeaveState();

private:
    // Event queue
    EventQueue eventQueue;

    // List widget for events
    QListWidget *listWidget;
};

#endif // EVENTQUEUEWINDOWSTATE_H
