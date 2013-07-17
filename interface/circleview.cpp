#include <GL/glut.h>
#include "circleview.h"
#include <sstream>
#include <CGAL/Random.h>
#include <QGLViewer/qglviewer.h>

static CGAL::Random randgen;

CircleView::CircleView()
{
    color.setRed(randgen.get_int(0, 255));
    color.setGreen(randgen.get_int(0, 255));
    color.setBlue(randgen.get_int(0, 255));
}

CircleView CircleView::fromCircle(const CircleHandle &ch)
{
    CircleView cv;
    cv.handle = ch;

    // Compute approximate data
    using CGAL::to_double;
    cv.radius = std::sqrt(to_double(ch->squared_radius()));
    cv.x = to_double(ch->center().x());
    cv.y = to_double(ch->center().y());
    cv.z = to_double(ch->center().z());

    // Compute approximate angles with x/y axes
    Vector_3 v = ch->supporting_plane().orthogonal_vector();
    Vector_3 v_xz(v.x(), 0, v.z()), v_yz(0, v.y(), v.z());
    cv.alpha = std::acos(to_double(Vector_3(0, 0, 1)*v_yz) / std::sqrt(to_double(v_yz.squared_length())));
    cv.beta = std::acos(to_double(Vector_3(0, 0, 1)*v_xz) / std::sqrt(to_double(v_xz.squared_length())));
    // ...transformed to be correct rotation angles
    cv.beta *= -1.;
    // ...these are needed in degrees
    cv.alpha *= 180. / M_PI;
    cv.beta *= 180. / M_PI;
    return cv;
}

QString CircleView::asString() const
{
    // Make view text
    std::ostringstream oss;
    oss << "[" << x << ", " << y
        << ", " << z << "], " << radius;

    return QString(oss.str().c_str());
}

bool CircleView::operator<(const CircleView &sv) const
{ return handle < sv.handle; }

void CircleView::draw(QGLViewer *viewer) const
{
    viewer->qglColor(color);
    glPushMatrix();
        glTranslated(x, y, z);
        glRotated(alpha, 1, 0, 0);
        glRotated(beta, 0, 1, 0);
        glutSolidTorus(0.01, radius, 15, 30);
    glPopMatrix();
}
