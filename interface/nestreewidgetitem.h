#ifndef NESTREEWIDGETITEM_H
#define NESTREEWIDGETITEM_H

#include "estreewidgetitem.h"
#include "eventqueue.h"
#include "sphereview.h"

class NESTreeWidgetItem : public ESTreeWidgetItem
{
public:
    NESTreeWidgetItem(const NormalEventSite &nes, QTreeWidget *parent = 0);
    ~NESTreeWidgetItem();
    void draw(QGLViewer *viewer);

private:
    NormalEventSite nes;
    SphereView pointSv;
};

#endif // NESTREEWIDGETITEM_H
