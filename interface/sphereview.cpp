#include <GL/glut.h>
#include "sphereview.h"
#include <sstream>
#include <QGLViewer/qglviewer.h>

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
        glutSolidSphere(radius, 20, 20);
    glPopMatrix();
}
