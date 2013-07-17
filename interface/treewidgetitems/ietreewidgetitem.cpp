#include "treewidgetitems/ietreewidgetitem.h"

IETreeWidgetItem::IETreeWidgetItem(const IntersectionEvent & ie, QTreeWidget *parent):
    DrawableTreeWidgetItem(parent), ie(ie)
{
    std::ostringstream oss;
    oss << "Intersection event";
    oss << " (";

    // String representation of intersection type
    if (ie.type == IntersectionEvent::Tangency)
    { oss << "tangency"; }
    else if (ie.type == IntersectionEvent::Largest_crossing)
    { oss << "largest crossing"; }
    else
    { Q_ASSERT(ie.type == IntersectionEvent::Smallest_crossing);
        oss << "smallest crossing"; }
    oss << ")";

    setText(0, QString(oss.str().c_str()));
}

IETreeWidgetItem::~IETreeWidgetItem() {}

void IETreeWidgetItem::draw(QGLViewer *viewer) const
{
}
