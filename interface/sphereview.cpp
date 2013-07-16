#include <GL/glew.h>
#include "sphereview.h"
#include <sstream>
#include <QGLViewer/qglviewer.h>

QString SphereView::asString() const
{
    // Compute approximate data
    qglviewer::Vec pos = frame.position();

    // Make view text
    std::ostringstream oss;
    oss << "[" << pos.x << ", " << pos.y
        << ", " << pos.z << "], " << radius;
    return QString(oss.str().c_str());
}

bool SphereView::operator<(const SphereView &sv) const
{ return handle < sv.handle; }

void SphereView::draw(QGLViewer *viewer) const
{
    glPushMatrix();
        viewer->qglColor(color);
        glMultMatrixd(frame.matrix());
        GLUquadricObj * sphere = gluNewQuadric();
        gluQuadricDrawStyle(sphere, GLU_SILHOUETTE);
        gluQuadricNormals(sphere, GLU_SMOOTH);
        gluQuadricOrientation(sphere, GLU_OUTSIDE);
        gluSphere(sphere, radius, 20, 20);
        gluDeleteQuadric(sphere);
    glPopMatrix();
}
