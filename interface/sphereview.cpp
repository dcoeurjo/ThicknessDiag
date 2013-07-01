#include "sphereview.h"

SphereView::SphereView()
{
}

SphereView::SphereView(const SI::Sphere_handle &sh,
        const QColor &color, const qglviewer::Vec &pos, double radius):
    handle(sh), color(color),
    frame(), radius(radius)
{
    frame.setPosition(pos);
}

SphereView::~SphereView()
{
}


QString SphereView::asString() const
{
    // Compute approximate data
    using CGAL::to_double;
    const Point_3 &c = handle->center();
    double radius = std::sqrt(to_double(handle->squared_radius())),
            x = to_double(c.x()),
            y = to_double(c.y()),
            z = to_double(c.z());

    // Make view text
    std::ostringstream oss;
    oss << "[" << x << ", " << y
        << ", " << z << "], " << radius;
    return QString(oss.str().c_str());
}