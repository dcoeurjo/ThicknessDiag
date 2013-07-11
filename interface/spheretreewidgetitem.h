#ifndef SPHERETREEWIDGETITEM_H
#define SPHERETREEWIDGETITEM_H

#include "drawabletreewidgetitem.h"
#include "sphereview.h"

class SphereTreeWidgetItem : public DrawableTreeWidgetItem
{
public:
    SphereTreeWidgetItem(const SphereView &sv, QTreeWidget *parent = 0);
    ~SphereTreeWidgetItem();

    void draw(QGLViewer *viewer);

private:
    const SphereView &sv;
};

#endif // SPHERETREEWIDGETITEM_H
