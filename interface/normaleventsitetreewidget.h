#ifndef NORMALEVENTSITETREEWIDGET_H
#define NORMALEVENTSITETREEWIDGET_H

#include <QTreeWidgetItem>
#include "eventqueue.h"

class NormalEventSiteTreeWidget : public QTreeWidgetItem
{
public:
    NormalEventSiteTreeWidget(const NormalEventSite &nes, QTreeWidget *parent = 0);
    ~NormalEventSiteTreeWidget();

private:
    NormalEventSite nes;
};

#endif // NORMALEVENTSITETREEWIDGET_H
