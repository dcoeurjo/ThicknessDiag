#include <GL/glut.h>
#include "circleview.h"
#include <sstream>
#include <CGAL/Random.h>
#include <QGLViewer/qglviewer.h>

static CGAL::Random randgen;

// Helper function computing the approximate signed angle
// between two vectors -> (a, b)
//
// Concept: vectors "a" and "b" are already projected along
// the plane defined by "normal"
static double signedAngle(const Vector_3 & a, const Vector_3 & b,
                          const Vector_3 & normal)
{
    using CGAL::to_double;
    Q_ASSERT(a*normal == 0 && b*normal == 0);
    Vector_3 a_b = CGAL::cross_product(a, b);
    double angle = std::atan2(std::sqrt(to_double(a_b.squared_length())),
        to_double(a*b) / std::sqrt(to_double(a.squared_length()*b.squared_length())));
    return normal*a_b >= 0 ? angle : -angle;
}

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
    std::cout << "Normal vector : " << "coords = (" << to_double(v.x())
              << ", " << to_double(v.y()) << "," << to_double(v.z())
              << "), length = " << to_double(v.squared_length()) << std::endl;
    cv.alpha = signedAngle(Vector_3(0, v.y(), v.z()), Vector_3(0, 0, 1), Vector_3(1, 0, 0));
    cv.beta = signedAngle(Vector_3(0, 0, 1), Vector_3(v.x(), 0, v.z()), Vector_3(0, 1, 0));
    cv.gamma = signedAngle(Vector_3(1, 0, 0), Vector_3(v.x(), v.y(), 0), Vector_3(0, 0, 1));
    // ...these are needed in degrees
    cv.alpha *= 180. / M_PI;
    cv.beta *= 180. / M_PI;
    cv.gamma *= 180. / M_PI;
    std::cout << "alpha = " << cv.alpha << std::endl;
    std::cout << "beta = " << cv.beta << std::endl;
    std::cout << "gamma = " << cv.gamma << std::endl;
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
        glRotated(gamma, 0, 0, 1);
        glutSolidTorus(0.01, radius, 15, 30);
    glPopMatrix();
}
