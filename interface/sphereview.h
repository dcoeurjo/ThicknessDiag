#ifndef SPHEREVIEW_H
#define SPHEREVIEW_H

#include <QColor>

#include <QGLViewer/frame.h>

#include "sphereintersecter.h"

struct SphereView
{
    QString asString() const;

    SphereHandle handle;
    QColor color;
    qglviewer::Frame frame;
    double radius;
};

#endif // SPHEREVIEW_H
