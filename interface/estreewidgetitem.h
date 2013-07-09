#ifndef ESTREEWIDGETITEM_H
#define ESTREEWIDGETITEM_H

#include <QTreeWidgetItem>

class QGLViewer;

class ESTreeWidgetItem : public QTreeWidgetItem
{
public:
    ESTreeWidgetItem(QTreeWidget *parent=0);
    virtual ~ESTreeWidgetItem();
    virtual void draw(QGLViewer *viewer) = 0;
};

#endif // ESTREEWIDGETITEM_H
