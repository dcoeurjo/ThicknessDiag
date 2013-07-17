#ifndef IETREEWIDGETITEM_H
#define IETREEWIDGETITEM_H

#include "../eventqueue.h"
#include "drawabletreewidgetitem.h"

class IETreeWidgetItem : public DrawableTreeWidgetItem
{
public:
    IETreeWidgetItem(const IntersectionEvent & ie, QTreeWidget *parent=0);
    ~IETreeWidgetItem();
    void draw(QGLViewer *viewer) const;

private:
    const IntersectionEvent & ie;
};

#endif // IETREEWIDGETITEM_H
