#ifndef CUSTOMVIEWER_H
#define CUSTOMVIEWER_H

#include <QGLViewer/qglviewer.h>

class CustomViewer : public QGLViewer
{
public:
    CustomViewer(QWidget *parent = 0);

protected:
    void init();
    void initializeGL();
};

#endif // CUSTOMVIEWER_H
