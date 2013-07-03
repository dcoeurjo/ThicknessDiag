#include <GL/glew.h>

#include "windowstatewidget.h"

#include <QColor>
#include <QGLViewer/frame.h>

#include <CGAL/Random.h>

#include "window.h"
#include "windowstate.h"

WindowStateWidget::WindowStateWidget(Window *window) :
    QWidget(window), siProxyInstance(window->siProxy())
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

void WindowStateWidget::setStatus(const QString &status)
{ w().showStatus(status); }

WindowStateFactory WindowStateWidget::factory()
{ return WindowStateFactory(*this); }

void WindowStateWidget::onStateChanged(WindowState &state)
{
}
