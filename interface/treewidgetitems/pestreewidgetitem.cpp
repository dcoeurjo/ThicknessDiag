#include "pestreewidgetitem.h"
#include <sstream>

PESTreeWidgetItem::PESTreeWidgetItem(const PolarEventSite & pes, QTreeWidget *parent)
{
    std::ostringstream oss;
    oss << "Polar event site";
    oss << " (";

    // north/south
    if (pes.event().is_north())
    { oss << "north"; }
    else
    { Q_ASSERT(pes.event().is_south());
        oss << "south"; }
    oss << ", ";
    // start/end
    if (pes.event().is_start())
    { oss << "start"; }
    else
    { Q_ASSERT(pes.event().is_end());
        oss << "end"; }
    oss << ")";

    setText(0, QString(oss.str().c_str()));
}

PESTreeWidgetItem::~PESTreeWidgetItem() {}

void PESTreeWidgetItem::draw(QGLViewer *viewer) const
{
}
