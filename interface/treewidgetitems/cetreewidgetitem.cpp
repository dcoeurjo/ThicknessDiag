#include "treewidgetitems/cetreewidgetitem.h"

CETreeWidgetItem::CETreeWidgetItem(const CriticalEvent & ce, QTreeWidget *parent):
    DrawableTreeWidgetItem(parent)
{
    std::ostringstream oss;
    oss << "Critical event";
    oss << "(";
    if (ce.is_start())
    {
        oss << "start";
    }
    else
    {
        Q_ASSERT(ce.is_end());
        oss << "end";
    }
    oss << ")";
    setText(0, QString(oss.str().c_str()));
}

CETreeWidgetItem::~CETreeWidgetItem()
{
}

void CETreeWidgetItem::draw(QGLViewer *viewer) const
{
}
