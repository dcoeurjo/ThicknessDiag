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

#include "eventqueuebuilder.h"
#include "selectspheredialog.h"
#include "spheretreewidgetitem.h"

EventQueueWindowState::EventQueueWindowState(WindowStateWidget &wsw):
    WindowState(wsw, tr("Event Queue")) {}

EventQueueWindowState::~EventQueueWindowState()
{ delete bottomWidget; }

void EventQueueWindowState::setup()
{
    // Create actions
    QAction *buildAction = new QAction(tr("Build"), this);
    buildAction->setShortcut(Qt::CTRL + Qt::Key_B);
    QObject::connect(buildAction, SIGNAL(triggered()),
                         this, SLOT(buildEventQueue()));
    QAction *selectSphereAction = new QAction(tr("Select sphere"), this);
    QObject::connect(selectSphereAction, SIGNAL(triggered()),
                     this, SLOT(selectSphere()));

    // Bottom widget
    bottomWidget = new QWidget();
    bottomWidget->hide();
    bottomWidget->setMaximumHeight(200);
    QHBoxLayout *horizontalLayout = new QHBoxLayout(bottomWidget);
    horizontalLayout->setContentsMargins(1, 1, 1, 1);

    // list widget
    treeWidget = new QTreeWidget(bottomWidget);
    treeWidget->setColumnCount(1);
    treeWidget->setHeaderLabel(tr("Event queue display"));
    horizontalLayout->addWidget(treeWidget);

    // button group
    QGroupBox *buttonGroupBox = new QGroupBox(bottomWidget);
    QVBoxLayout *buttonGroupLayout = new QVBoxLayout(buttonGroupBox);
    QPushButton *selectSphereButton = new QPushButton(tr("Select a sphere"));
    QObject::connect(selectSphereButton, SIGNAL(clicked()),
                     selectSphereAction, SIGNAL(triggered()));
    QPushButton *buildButton = new QPushButton(tr("Build event queue"));
    QObject::connect(buildButton, SIGNAL(clicked()),
                     buildAction, SIGNAL(triggered()));
    buttonGroupLayout->addWidget(selectSphereButton);
    buttonGroupLayout->addWidget(buildButton);
    horizontalLayout->addWidget(buttonGroupBox);
}

void EventQueueWindowState::onEnterState()
{
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
    // Do nothing if no sphere is selected
    if (selectedSphere.handle.is_null())
    { return; }

    selectedSphere.draw(wsw.viewer());
}

void EventQueueWindowState::selectSphere()
{
    SelectSphereDialog ssd;
    SphereIteratorRange spheres(siProxy.directAccess());
    for (SphereIterator it = spheres.begin(); it != spheres.end(); it++)
    { ssd.addSphere(wsw.sphereView(*it)); }

    // Execute and assign selected
    if (ssd.exec())
    {
        treeWidget->clear();
        selectedSphere = *ssd.selectedSphere;
        SphereTreeWidgetItem *sphereItem = new SphereTreeWidgetItem(selectedSphere, treeWidget);
        treeWidget->addTopLevelItem(sphereItem);
        updateUI();
    }
}

void EventQueueWindowState::buildEventQueue()
{
    // Select a sphere if non is yet selected
    if (selectedSphere.handle.is_null())
    { selectSphere(); }

    // Fail with message if no sphere is selected
    if (selectedSphere.handle.is_null())
    {
        QMessageBox::warning(&wsw, tr("No sphere selected"),
                             tr("You need to select a sphere in order to build its event queue"));
    }
    else
    {
        // Build event queue
        eventQueue = EventQueueBuilder()(siProxy.directAccess(), selectedSphere.handle);

        // Clear sphere item's children
        QTreeWidgetItem *sphereItem = treeWidget->topLevelItem(0);
        for (int i = 0; i < sphereItem->childCount(); i++)
        { sphereItem->removeChild(sphereItem->child(i)); }

        // Add its new children
        for (EventSiteType evsType = eventQueue.next_event(); evsType != EventQueue::None;
             evsType = eventQueue.next_event())
        {
            if (evsType == EventQueue::Normal)
            {
                NormalEventSite event = eventQueue.pop_normal();
            }
            else
            {
                Q_ASSERT(evsType == EventQueue::Polar);
                PolarEventSite event = eventQueue.pop_polar();
            }
        }
    }
}

void EventQueueWindowState::updateUI()
{
    bottomWidget->update();
    wsw.viewer()->update();
}
