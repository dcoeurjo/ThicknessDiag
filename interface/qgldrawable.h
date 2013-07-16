#ifndef QGLDRAWABLE_H
#define QGLDRAWABLE_H

class QGLViewer;

struct QGLDrawable
{
    virtual void draw(QGLViewer *viewer) const = 0;
};

#endif // QGLDRAWABLE_H
