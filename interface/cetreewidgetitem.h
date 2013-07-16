#ifndef CETREEWIDGETITEM_H
#define CETREEWIDGETITEM_H

#include "eventqueue.h"
#include "drawabletreewidgetitem.h"

class CETreeWidgetItem : public DrawableTreeWidgetItem
{
public:
    CETreeWidgetItem(const CriticalEvent & ce, QTreeWidget *parent = 0);
    ~CETreeWidgetItem();
    void draw(QGLViewer *viewer) const;
};

#endif // CETREEWIDGETITEM_H
