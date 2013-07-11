#ifndef EVENTQUEUEWINDOWSTATE_H
#define EVENTQUEUEWINDOWSTATE_H

#include <QSet>
#include "windowstatewithmenu.h"
#include "eventqueue.h"

class QTreeWidget;
class DrawableTreeWidgetItem;

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
    void buildEventQueue();
    void updateDrawables();

private:
    // Helper for updating the UI
    void updateUI();

    // Event queue
    EventQueue eventQueue;

    // Tree items to display
    QSet<DrawableTreeWidgetItem*> drawableItems;

    // UI
    QWidget *bottomWidget;
    QTreeWidget *treeWidget;
};

#endif // EVENTQUEUEWINDOWSTATE_H
