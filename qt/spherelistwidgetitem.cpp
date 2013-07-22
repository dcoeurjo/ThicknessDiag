#include "spherelistwidgetitem.h"

SphereListWidgetItem::SphereListWidgetItem(const SphereView &sv, QListWidget *listWidget):
    QListWidgetItem(sv.asString(), listWidget, UserType), svMember(sv) {}

const SphereView& SphereListWidgetItem::sv() const
{ return svMember; }