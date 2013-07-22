#include "windowstatewidget.h"
#include <iostream>
#include <QMenu>
#include <QColor>
#include <QMenuBar>
#include <QStatusBar>
#include <QGridLayout>
#include <QMainWindow>
#include "windowstate.h"
#include "../window.h"
#include "../customviewer.h"

WindowStateWidget::WindowStateWidget(SphereIntersecterProxy &siProxy, QMainWindow *window) :
    QWidget(window), siProxyInstance(siProxy),
    currentState(0), nbStates(0)
{
    // Setup UI elements
    stateMenu = mw().menuBar()->addMenu(tr("Window Mode"));
    viewerMember = new CustomViewer(this);
    viewerMember->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Connect to sphere addition/deletion
    QObject::connect(&siProxyInstance, SIGNAL(sphereAdded(Sphere_3)),
                     this, SLOT(onSphereAdded(Sphere_3)));
    QObject::connect(&siProxyInstance, SIGNAL(sphereRemoved(Sphere_3)),
                     this, SLOT(onSphereRemoved(Sphere_3)));

    // Connect to viewer update
    QObject::connect(viewerMember, SIGNAL(drawNeeded()),
                     this, SLOT(drawViewer()));

    // Add widget states
    WindowStateFactory stateFactory = factory();
    WindowState &spheresState = stateFactory.makeState(WindowStateFactory::SPHERES);
    addState(spheresState);
    addState(stateFactory.makeState(WindowStateFactory::EVENT_QUEUE));
    changeState(spheresState);

    // Show startup message
    setStatus(tr("Loaded"));
}

WindowStateWidget::~WindowStateWidget() {}

void WindowStateWidget::onSphereAdded(const Sphere_3 &s)
{
    // Construct SphereHandle directly (SphereIntersecterProxy concept)
    SphereHandle sh(s);

    // Add list to sphere handle vector
    sphereViews.insert(SphereView::fromSphere(sh));

    // Update the scene so that it includes the sphere
    BoundingBox sceneBbox = siProxy().directAccess().bbox();
    qglviewer::Vec min(sceneBbox.xmin(), sceneBbox.ymin(),
                       sceneBbox.zmin()),
            max(sceneBbox.xmax(), sceneBbox.ymax(),
                sceneBbox.zmax());
    viewerMember->setSceneRadius((max - min).norm() / 2.0);
}

void WindowStateWidget::onSphereRemoved(const Sphere_3 &s)
{
    // Construct temp sphere view for lookup
    SphereView sv;
    sv.handle = SphereHandle(s);

    // Find and erase sphere view
    SphereViews::iterator it = sphereViews.find(sv);
    Q_ASSERT(it != sphereViews.end());
    sphereViews.erase(it);
}

void WindowStateWidget::setStatus(const QString &status, int timeout)
{ mw().statusBar()->showMessage(status, timeout);  }

void WindowStateWidget::addState(WindowState &state)
{
    QAction *enterAction = state.enterAction();
    stateMenu->addAction(enterAction);
    if (nbStates++ < 10)
    {
        std::ostringstream oss;
        oss << "CTRL+" << nbStates % 10;
        enterAction->setShortcut(QKeySequence(QString(oss.str().c_str())));
    }
}

void WindowStateWidget::changeState(WindowState &state)
{
    if (currentState != 0)
    {
        currentState->leaveState();
        currentState->enterAction()->setEnabled(true);
    }
    currentState = &state;
    currentState->enterAction()->setEnabled(false);
    currentState->enterState();
    viewerMember->update();

    // Display a status message
    std::ostringstream oss;
    oss << "Entered '" << currentState->name().toStdString() << "' mode";
    setStatus(tr(oss.str().c_str()));
}

const SphereView& WindowStateWidget::sphereView(const SphereHandle& sh)
{
    // Construct temp sphere view
    SphereView tmp_sv;
    tmp_sv.handle = sh;

    // Find temp sphere view and return it, asserting for failure
    SphereViews::const_iterator it = sphereViews.find(tmp_sv);
    Q_ASSERT(it != sphereViews.end());
    return *it;
}


void WindowStateWidget::drawViewer()
{
    if (currentState != 0)
    { currentState->draw(); }
}
