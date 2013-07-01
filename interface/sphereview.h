#ifndef SPHEREVIEW_H
#define SPHEREVIEW_H

#include <QColor>

#include <QGLViewer/frame.h>

#include "sphereintersecter.h"

struct SphereView
{
    typedef SphereIntersecter SI;

    SphereView();
    SphereView(const SI::Sphere_handle &sh, const QColor &color,
               const qglviewer::Vec &pos, double radius);
    ~SphereView();

    QString asString() const;

    SI::Sphere_handle handle;
    QColor color;
    qglviewer::Frame frame;
    double radius;
};

#endif // SPHEREVIEW_H
