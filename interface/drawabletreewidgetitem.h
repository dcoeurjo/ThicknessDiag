#ifndef ESTREEWIDGETITEM_H
#define ESTREEWIDGETITEM_H

#include <QTreeWidgetItem>

class QGLViewer;

class DrawableTreeWidgetItem : public QTreeWidgetItem
{
public:
    DrawableTreeWidgetItem(QTreeWidget *parent=0);
    virtual ~DrawableTreeWidgetItem();
    virtual void draw(QGLViewer *viewer) = 0;
};

#endif // ESTREEWIDGETITEM_H
