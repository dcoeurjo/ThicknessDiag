#include "estreewidgetitem.h"

ESTreeWidgetItem::ESTreeWidgetItem(QTreeWidget *parent):
    QTreeWidgetItem(parent, UserType) {}

ESTreeWidgetItem::~ESTreeWidgetItem() {}