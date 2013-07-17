#ifndef CETREEWIDGETITEM_H
#define CETREEWIDGETITEM_H

#include "drawabletreewidgetitem.h"
#include "../eventqueue.h"
#include "../circleview.h"

class CETreeWidgetItem : public DrawableTreeWidgetItem
{
public:
    CETreeWidgetItem(const CriticalEvent & ce, QTreeWidget *parent = 0);
    ~CETreeWidgetItem();
    void draw(QGLViewer *viewer) const;

private:
    CircleView cv;
};

#endif // CETREEWIDGETITEM_H
