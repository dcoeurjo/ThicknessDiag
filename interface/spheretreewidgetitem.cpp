#include "spheretreewidgetitem.h"

SphereTreeWidgetItem::SphereTreeWidgetItem(const SphereView &sv, QTreeWidget *parent) :
    QTreeWidgetItem(parent), sv(sv)
{
    setText(0, sv.asString());
}

SphereTreeWidgetItem::~SphereTreeWidgetItem() {}
