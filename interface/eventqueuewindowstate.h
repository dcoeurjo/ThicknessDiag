#ifndef EVENTQUEUEWINDOWSTATE_H
#define EVENTQUEUEWINDOWSTATE_H

#include "windowstate.h"
#include "eventqueue.h"

class QTreeWidget;

class EventQueueWindowState : public WindowState
{
    Q_OBJECT

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
    void selectSphere();

private:
    // Helper for updating the UI
    void updateUI();

    // Event queue
    EventQueue eventQueue;

    // Selected sphere
    SphereView selectedSphere;

    // UI
    QWidget *bottomWidget;
    QTreeWidget *treeWidget;
};

#endif // EVENTQUEUEWINDOWSTATE_H
