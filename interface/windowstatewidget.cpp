#include <GL/glew.h>

#include "windowstatewidget.h"

#include <QColor>
#include <QMenuBar>
#include <QStatusBar>
#include <QMainWindow>

#include <QGLViewer/frame.h>

#include <CGAL/Random.h>

#include "window.h"
#include "windowstate.h"

WindowStateWidget::WindowStateWidget(SphereIntersecterProxy &siProxy, QMainWindow *window) :
    QWidget(window), siProxyInstance(siProxy),
    currentState(0)
{
    // Setup UI elements
    // TODO

    // Connect to sphere addition/deletion
    QObject::connect(&siProxyInstance, SIGNAL(sphereAdded(SphereHandle)),
                     this, SLOT(onSphereAdded(SphereHandle)));
    QObject::connect(&siProxyInstance, SIGNAL(sphereRemoved(SphereHandle)),
                     this, SLOT(onSphereRemoved(SphereHandle)));

    // Connect to viewer update
    QObject::connect(viewer, SIGNAL(drawNeeded()),
                     this, SIGNAL(drawNeeded(viewer)));
}

void WindowStateWidget::onSphereAdded(const SphereHandle &sh)
{
    // Compute approximate data
    using CGAL::to_double;
    const Point_3 &c = sh->center();
    double radius = std::sqrt(to_double(sh->squared_radius())),
            x = to_double(c.x()),
            y = to_double(c.y()),
            z = to_double(c.z());

    // Make sphere color
    QColor color;
    CGAL::Random randgen;
    color.setRed(randgen.get_int(0, 255));
    color.setGreen(randgen.get_int(0, 255));
    color.setBlue(randgen.get_int(0, 255));

    // Finally, make sphere
    SphereView sv;
    sv.handle = sh;
    sv.color = color;
    sv.frame.setPosition(x, y, z);
    sv.radius = radius;

    // Add list to sphere handle vector
    sphereViews[sh] = sv;

    // Update the scene so that it includes the sphere
    BoundingBox sceneBbox = siProxy().directAccess().bbox();
    qglviewer::Vec min(sceneBbox.xmin(), sceneBbox.ymin(),
                       sceneBbox.zmin()),
            max(sceneBbox.xmax(), sceneBbox.ymax(),
                sceneBbox.zmax());
    viewer->setSceneRadius((max - min).norm() / 2.0);
}

void WindowStateWidget::onSphereRemoved(const SphereHandle &sh)
{ sphereViews.erase(sh); }

void WindowStateWidget::setStatus(const QString &status, int timeout)
{ mw().statusBar()->showMessage(status, timeout);  }

WindowStateFactory WindowStateWidget::factory()
{ return WindowStateFactory(*this); }

void WindowStateWidget::addState(WindowState &state)
{
    QAction *enterAction = state.makeEnterAction();
    mw().menuBar()->addAction(enterAction);
}

void WindowStateWidget::changeState(WindowState &state)
{
    if (currentState != 0)
    { currentState->leaveState(); }
    currentState = &state;
    currentState->enterState();
}

const SphereView& WindowStateWidget::sphereView(const SphereHandle& sh)
{
    SphereViews::const_iterator it = sphereViews.find(sh);
    Q_ASSERT(it != sphereViews.end());
    return it->second;
}
