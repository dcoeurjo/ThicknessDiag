#include "drawabletreewidgetitem.h"

DrawableTreeWidgetItem::DrawableTreeWidgetItem(QTreeWidget *parent):
    QTreeWidgetItem(parent, UserType) {}

DrawableTreeWidgetItem::~DrawableTreeWidgetItem() {}
