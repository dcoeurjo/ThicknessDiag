#ifndef NESTREEWIDGETITEM_H
#define NESTREEWIDGETITEM_H

#include "../eventqueue.h"
#include "../sphereview.h"
#include "drawabletreewidgetitem.h"

class NESTreeWidgetItem : public DrawableTreeWidgetItem
{
public:
    NESTreeWidgetItem(const NormalEventSite &nes, QTreeWidget *parent = 0);
    ~NESTreeWidgetItem();
    void draw(QGLViewer *viewer) const;

private:
    SphereView pointSv;
};

#endif // NESTREEWIDGETITEM_H
