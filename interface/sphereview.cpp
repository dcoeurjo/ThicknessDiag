#include <GL/glut.h>
#include "sphereview.h"
#include <sstream>
#include <CGAL/Random.h>
#include <QGLViewer/qglviewer.h>

static CGAL::Random randgen;

SphereView::SphereView()
{
    color.setRed(randgen.get_int(0, 255));
    color.setGreen(randgen.get_int(0, 255));
    color.setBlue(randgen.get_int(0, 255));
}

SphereView SphereView::fromSphere(const SphereHandle &sh)
{
    using CGAL::to_double;
    SphereView sv;
    sv.handle = sh;
    sv.radius = std::sqrt(to_double(sh->squared_radius()));
    sv.x = to_double(sh->center().x());
    sv.y = to_double(sh->center().y());
    sv.z = to_double(sh->center().z());
    return sv;
}

QString SphereView::asString() const
{
    // Make view text
    std::ostringstream oss;
    oss << "[" << x << ", " << y
        << ", " << z << "], " << radius;
    return QString(oss.str().c_str());
}

bool SphereView::operator<(const SphereView &sv) const
{ return handle < sv.handle; }

void SphereView::draw(QGLViewer *viewer) const
{
    viewer->qglColor(color);
    glPushMatrix();
        glTranslatef(x, y, z);
        glutWireSphere(radius, 20, 20);
    glPopMatrix();
}
