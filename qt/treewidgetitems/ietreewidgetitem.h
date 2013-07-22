#ifndef IETREEWIDGETITEM_H
#define IETREEWIDGETITEM_H

#include "drawabletreewidgetitem.h"
#include "../eventqueue.h"
#include "../circleview.h"

class IETreeWidgetItem : public DrawableTreeWidgetItem
{
public:
    IETreeWidgetItem(const IntersectionEvent & ie, QTreeWidget *parent=0);
    ~IETreeWidgetItem();
    void draw(QGLViewer *viewer) const;

private:
    std::pair<CircleView, CircleView> cvPair;
};

#endif // IETREEWIDGETITEM_H
