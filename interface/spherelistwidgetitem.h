#ifndef SPHERELISTWIDGETITEM_H
#define SPHERELISTWIDGETITEM_H

#include <QListWidgetItem>

#include "sphereview.h"

class SphereListWidgetItem : public QListWidgetItem
{
public:
    SphereListWidgetItem(const SphereView &sv, QListWidget *listWidget);

    // Getter for sphere view
    const SphereView& sv() const;

private:
    SphereView svMember;
};

#endif // SPHERELISTWIDGETITEM_H
