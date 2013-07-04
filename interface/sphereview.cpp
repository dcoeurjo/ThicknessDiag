#include "sphereview.h"
#include <sstream>

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