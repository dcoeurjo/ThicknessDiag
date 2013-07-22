#include "ietreewidgetitem.h"

IETreeWidgetItem::IETreeWidgetItem(const IntersectionEvent & ie, QTreeWidget *parent):
    DrawableTreeWidgetItem(parent)
{
    // Build corresponding circle views for both CE's circles
    cvPair.first = CircleView::fromCircle(ie.circles.first);
    cvPair.second = CircleView::fromCircle(ie.circles.second);

    // Build string representation
    std::ostringstream oss;
    oss << "Intersection event";
    oss << " (";
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
    cvPair.first.draw(viewer);
    cvPair.second.draw(viewer);
}
