#ifndef PESTREEWIDGETITEM_H
#define PESTREEWIDGETITEM_H

#include "drawabletreewidgetitem.h"
#include "../eventqueue.h"

class PESTreeWidgetItem : public DrawableTreeWidgetItem
{
public:
    PESTreeWidgetItem(const PolarEventSite & pes, QTreeWidget *parent=0);
    ~PESTreeWidgetItem();
    void draw(QGLViewer *viewer) const;

private:
    PolarEventSite pes;
};

#endif // PESTREEWIDGETITEM_H
