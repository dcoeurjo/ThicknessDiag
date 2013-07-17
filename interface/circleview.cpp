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
    using CGAL::to_double;
    CircleView cv;
    cv.handle = ch;
    cv.radius = std::sqrt(to_double(ch->squared_radius()));
    cv.x = to_double(ch->center().x());
    cv.y = to_double(ch->center().y());
    cv.z = to_double(ch->center().z());
    return cv;
}

QString CircleView::asString() const
{
    // Make view text
    std::ostringstream oss;
    oss << "[" << x << ", " << y
        << ", " << z << "], " << radius;

    // TODO add plane display

    return QString(oss.str().c_str());
}

bool CircleView::operator<(const CircleView &sv) const
{ return handle < sv.handle; }

void CircleView::draw(QGLViewer *viewer) const
{
    viewer->qglColor(color);
    glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidTorus(0.01, radius, 15, 30);
    glPopMatrix();
}
