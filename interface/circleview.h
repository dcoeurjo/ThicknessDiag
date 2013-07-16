#ifndef CIRCLEVIEW_H
#define CIRCLEVIEW_H

#include <QColor>
#include <QGLViewer/frame.h>
#include "qgldrawable.h"
#include "sphereintersecter.h"

struct CircleView : public QGLDrawable
{
    // Get the string representation of a circle view
    QString asString() const;

    // Compare a sphere view by its handle
    bool operator<(const CircleView &sv) const;

    // Draw the circle
    void draw(QGLViewer *viewer) const;

    // Handle to sphere
    CircleHandle handle;

    // Circle's display color
    QColor color;

    // Circle approx' descriptors (x, y, z), radius
    qglviewer::Frame frame;
    double radius;
    // TODO
};

#endif // CIRCLEVIEW_H