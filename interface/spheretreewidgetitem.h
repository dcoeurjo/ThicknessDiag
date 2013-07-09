#ifndef SPHERETREEWIDGETITEM_H
#define SPHERETREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include "sphereview.h"

class SphereTreeWidgetItem : public QTreeWidgetItem
{
public:
    SphereTreeWidgetItem(const SphereView &sv, QTreeWidget *parent = 0);
    ~SphereTreeWidgetItem();

    const SphereView& sphereView() const
    { return sv; }

private:
    const SphereView &sv;
};

#endif // SPHERETREEWIDGETITEM_H
