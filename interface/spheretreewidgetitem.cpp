#include "spheretreewidgetitem.h"

SphereTreeWidgetItem::SphereTreeWidgetItem(const SphereView &sv, QTreeWidget *parent) :
    DrawableTreeWidgetItem(parent), sv(sv)
{
    setText(0, "Sphere " + sv.asString());
}

SphereTreeWidgetItem::~SphereTreeWidgetItem() {}

void SphereTreeWidgetItem::draw(QGLViewer *viewer)
{ sv.draw(viewer); }
