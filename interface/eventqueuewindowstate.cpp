#include "eventqueuewindowstate.h"

#include <QAction>
#include <QListWidget>
#include <QVBoxLayout>

#include <QGLViewer/qglviewer.h>

#include "eventqueuebuilder.h"

EventQueueWindowState::EventQueueWindowState(WindowStateWidget &wsw):
    WindowState(wsw, tr("Event Queue")) {}

EventQueueWindowState::~EventQueueWindowState()
{ delete listWidget; }

void EventQueueWindowState::setup()
{
    // Create actions
    QAction *buildAction = new QAction(tr("Build"), this);
    buildAction->setShortcut(Qt::CTRL + Qt::Key_B);
    QObject::connect(buildAction, SIGNAL(triggered()),
                         this, SLOT(buildEventQueue()));

    // Setup list widget
    listWidget = new QListWidget();
    listWidget->setMaximumHeight(200);
    QObject::connect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                     this, SLOT(eventSelectionChanged(QListWidgetItem*,QListWidgetItem*)));
}

void EventQueueWindowState::onEnterState()
{
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(2, -1, 2, -1);
    verticalLayout->addWidget(wsw.viewer());
    verticalLayout->addWidget(listWidget);
    wsw.setLayout(verticalLayout);
    listWidget->show();
}

void EventQueueWindowState::onLeaveState()
{
    QVBoxLayout *verticalLayout = dynamic_cast<QVBoxLayout*>(wsw.layout());
    Q_ASSERT(verticalLayout != 0);
    verticalLayout->removeWidget(wsw.viewer());
    verticalLayout->removeWidget(listWidget);
    verticalLayout->removeWidget(&wsw);
    delete verticalLayout;
    listWidget->hide();
}

void EventQueueWindowState::draw()
{
    // TODO
}

void EventQueueWindowState::buildEventQueue()
{
    EventQueueBuilder eqb;
    //eqb(siProxy, );
}

void EventQueueWindowState::eventSelectionChanged(QListWidgetItem *prev, QListWidgetItem *current)
{
}
