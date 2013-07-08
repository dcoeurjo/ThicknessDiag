#ifndef EVENTQUEUEWINDOWSTATE_H
#define EVENTQUEUEWINDOWSTATE_H

#include "windowstate.h"
#include "eventqueue.h"

class QListWidget;
class QListWidgetItem;

class EventQueueWindowState : public WindowState
{
public:
    explicit EventQueueWindowState(WindowStateWidget &wsw);
    ~EventQueueWindowState();

    void setup();
    void draw();

protected:
    void onEnterState();
    void onLeaveState();

private slots:
    void buildEventQueue();
    void eventSelectionChanged(QListWidgetItem *prev, QListWidgetItem *current);

private:
    // Event queue
    EventQueue eventQueue;

    // List widget for events
    QListWidget *listWidget;
};

#endif // EVENTQUEUEWINDOWSTATE_H
