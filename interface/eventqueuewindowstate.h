#ifndef EVENTQUEUEWINDOWSTATE_H
#define EVENTQUEUEWINDOWSTATE_H

#include "windowstatewithmenu.h"
#include "eventqueue.h"

class QTreeWidget;
class QTreeWidgetItem;
class ESTreeWidgetItem;

class EventQueueWindowState : public WindowStateWithMenu
{
    Q_OBJECT

public:
    EventQueueWindowState(WindowStateWidget &wsw);
    ~EventQueueWindowState();

    void setup();
    void draw();

protected:
    void onEnterState();
    void onLeaveState();

private slots:
    void updateSelectedEventSites();
    void buildEventQueue();
    void selectSphere();

private:
    // Helper for updating the UI
    void updateUI();

    // Event queue
    EventQueue eventQueue;

    // Selected sphere
    SphereView selectedSphere;

    // Event sites currently visualized
    QList<ESTreeWidgetItem*> eventSites;

    // UI
    QWidget *bottomWidget;
    QTreeWidget *treeWidget;
};

#endif // EVENTQUEUEWINDOWSTATE_H
