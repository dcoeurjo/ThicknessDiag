#ifndef SPHEREVIEW_H
#define SPHEREVIEW_H

#include <QColor>

#include <QGLViewer/frame.h>

#include "sphereintersecter.h"

class QGLViewer;

struct SphereView
{
    // Get the string representation of a sphere view
    QString asString() const;

    // Compare a sphere view by its handle
    bool operator<(const SphereView &sv) const;

    // Draw a sphere
    void draw(QGLViewer *viewer) const;

    // Handle to sphere
    SphereHandle handle;

    // Sphere's display color
    QColor color;

    // Sphere approx' descriptors (x, y, z), radius
    qglviewer::Frame frame;
    double radius;
};

#endif // SPHEREVIEW_H
