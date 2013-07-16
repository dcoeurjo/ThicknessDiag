#ifndef ESTREEWIDGETITEM_H
#define ESTREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include "qgldrawable.h"

class QGLViewer;

class DrawableTreeWidgetItem : public QTreeWidgetItem, public QGLDrawable
{
public:
    DrawableTreeWidgetItem(QTreeWidget *parent=0);
    virtual ~DrawableTreeWidgetItem();
};

#endif // ESTREEWIDGETITEM_H
