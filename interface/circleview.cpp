#include <GL/glew.h>
#include "circleview.h"
#include <sstream>
#include <QGLViewer/qglviewer.h>

QString CircleView::asString() const
{
    // Compute approximate data
    qglviewer::Vec pos = frame.position();

    // Make view text
    std::ostringstream oss;
    oss << "[" << pos.x << ", " << pos.y
        << ", " << pos.z << "], " << radius;

    // TODO add plane display

    return QString(oss.str().c_str());
}

bool CircleView::operator<(const CircleView &sv) const
{ return handle < sv.handle; }

void CircleView::draw(QGLViewer *viewer) const
{
    // TODO
}
