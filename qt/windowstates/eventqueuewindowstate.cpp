#include "eventqueuewindowstate.h"
#include <QAction>
#include <QGroupBox>
#include <QMessageBox>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QGLViewer/qglviewer.h>
#include "../eventqueuebuilder.h"
#include "../dialogs/selectspheredialog.h"
#include "../treewidgetitems/spheretreewidgetitem.h"
#include "../treewidgetitems/nestreewidgetitem.h"
#include "../treewidgetitems/pestreewidgetitem.h"
#include "../treewidgetitems/bpestreewidgetitem.h"

EventQueueWindowState::EventQueueWindowState(WindowStateWidget &wsw):
    WindowStateWithMenu(wsw, tr("Event Queue")) {}

EventQueueWindowState::~EventQueueWindowState()
{ delete bottomWidget; }

void EventQueueWindowState::setup()
{
    // Setup parent
    WindowStateWithMenu::setup();

    // Create actions
    QAction *buildAction = new QAction(tr("Build"), this);

    // Setup shortcuts
    buildAction->setShortcut(Qt::CTRL + Qt::Key_B);

    // Connect actions to slots
    QObject::connect(buildAction, SIGNAL(triggered()),
                         this, SLOT(buildEventQueue()));

    // Add actions to menu
    addAction(buildAction);

    // Bottom widget
    bottomWidget = new QWidget();
    bottomWidget->hide();
    bottomWidget->setMaximumHeight(200);
    QHBoxLayout *horizontalLayout = new QHBoxLayout(bottomWidget);
    horizontalLayout->setContentsMargins(1, 1, 1, 1);

    // list widget
    treeWidget = new QTreeWidget(bottomWidget);
    treeWidget->setColumnCount(1);
    treeWidget->setSelectionMode(QTreeWidget::ExtendedSelection);
    treeWidget->setHeaderLabel(tr("Event queue display"));
    QObject::connect(treeWidget, SIGNAL(itemSelectionChanged()),
                     this, SLOT(updateDrawables()));
    QObject::connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
                     this, SLOT(treeWidgetItemDoubleClicked(QTreeWidgetItem*,int)));
    horizontalLayout->addWidget(treeWidget);
}

void EventQueueWindowState::onEnterState()
{
    WindowStateWithMenu::onEnterState();
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(2, -1, 2, -1);
    verticalLayout->addWidget(wsw.viewer());
    verticalLayout->addWidget(bottomWidget);
    wsw.setLayout(verticalLayout);
    bottomWidget->show();
}

void EventQueueWindowState::onLeaveState()
{
    WindowStateWithMenu::onLeaveState();
    QVBoxLayout *verticalLayout = dynamic_cast<QVBoxLayout*>(wsw.layout());
    Q_ASSERT(verticalLayout != 0);
    verticalLayout->removeWidget(wsw.viewer());
    verticalLayout->removeWidget(bottomWidget);
    verticalLayout->removeWidget(&wsw);
    delete verticalLayout;
    bottomWidget->hide();
}

void EventQueueWindowState::draw()
{
    foreach(const DrawableTreeWidgetItem *drawableItem, drawableItems)
    { drawableItem->draw(wsw.viewer()); }
}

void EventQueueWindowState::buildEventQueue()
{
    // Select a sphere
    SelectSphereDialog ssd;
    SphereIteratorRange spheres(siProxy.directAccess());
    for (SphereIterator it = spheres.begin(); it != spheres.end(); it++)
    { ssd.addSphere(wsw.sphereView(*it)); }

    // Execute and assign selected
    if (ssd.exec() == false)
    { setStatus(tr("No sphere selected"));
      return; }

    // Apply for all selected spheres
    foreach (const SphereView *currentSelectedSphere, ssd.selectedSpheres())
    {
        // Remove last build of this sphere
        for (int i = 0; i < treeWidget->topLevelItemCount(); i++)
        {
            typedef SphereTreeWidgetItem STWI;
            STWI* sphereItem = reinterpret_cast<STWI*>(treeWidget->topLevelItem(i));
            if (currentSelectedSphere == &sphereItem->sphereView())
            { delete sphereItem;
              break; }
        }

        // Setup top level tree item (sphere selected)
        const SphereView &selectedSphere = *currentSelectedSphere;
        QTreeWidgetItem *sphereItem = new SphereTreeWidgetItem(selectedSphere,
                                                               treeWidget);
        treeWidget->addTopLevelItem(sphereItem);

        // Build event queue
        eventQueue = EventQueueBuilder()(siProxy.directAccess(),
                selectedSphere.handle);

        // Add its new children
        for (EventSiteType evsType = eventQueue.next_event();
             evsType != EventQueue::None; evsType = eventQueue.next_event())
        {
            QTreeWidgetItem *eventItem = 0;
            if (evsType == EventQueue::Normal)
            {
                NormalEventSite event = eventQueue.pop_normal();
                eventItem = new NESTreeWidgetItem(event);
            }
            else if (evsType == EventQueue::Bipolar)
            {
                BipolarEventSite event = eventQueue.pop_bipolar();
                eventItem = new BPESTreeWidgetItem(event);
            }
            else
            {
                Q_ASSERT(evsType == EventQueue::Polar);
                PolarEventSite event = eventQueue.pop_polar();
                eventItem = new PESTreeWidgetItem(event);
            }
            sphereItem->addChild(eventItem);
        }
    }
}

void EventQueueWindowState::updateUI()
{
    bottomWidget->update();
    wsw.viewer()->update();
}

void EventQueueWindowState::updateDrawables()
{
    drawableItems.clear();
    QList<QTreeWidgetItem*> selectedItems = treeWidget->selectedItems();
    for (int i = 0; i < selectedItems.count(); i++)
    {
        for (QTreeWidgetItem *item = selectedItems[i];
             item != 0; item = item->parent())
        {
            DrawableTreeWidgetItem *drawableItem = 0;
            drawableItem = dynamic_cast<DrawableTreeWidgetItem*>(item);
            if (drawableItem != 0)
            { drawableItems.insert(drawableItem); }
        }
    }
    updateUI();
}

void EventQueueWindowState::treeWidgetItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    typedef SphereTreeWidgetItem STWI;
    STWI *sphereItem = dynamic_cast<STWI*>(item);
    if (sphereItem != 0)
    {
        // Prevent double click from folding/unfolding
        sphereItem->setExpanded(!sphereItem->isExpanded());

        // Look/Center at sphere
        const SphereView &sv = sphereItem->sphereView();
        qglviewer::Vec c(sv.x, sv.y, sv.z);
        wsw.viewer()->setSceneCenter(c);
        wsw.viewer()->camera()->lookAt(c);

        // Finally, update the UI
        updateUI();
    }
}
