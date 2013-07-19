#ifndef BPESTREEWIDGETITEM_H
#define BPESTREEWIDGETITEM_H

#include "drawabletreewidgetitem.h"
#include "../eventqueue.h"

class BPESTreeWidgetItem : public DrawableTreeWidgetItem
{
public:
    BPESTreeWidgetItem(const BipolarEventSite & pes, QTreeWidget *parent=0);
    ~BPESTreeWidgetItem();
    void draw(QGLViewer *viewer) const;

private:
    BipolarEventSite bpes;
};

#endif // BPESTREEWIDGETITEM_H
