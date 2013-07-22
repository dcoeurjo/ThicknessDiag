#include "bpestreewidgetitem.h"
#include <sstream>

BPESTreeWidgetItem::BPESTreeWidgetItem(const BipolarEventSite & bpes, QTreeWidget *parent)
{
    std::ostringstream oss;
    oss << "Bipolar event site";
    oss << " (";

    // start/end
    if (bpes.event().is_start())
    { oss << "start"; }
    else
    { Q_ASSERT(bpes.event().is_end());
        oss << "end"; }
    oss << ")";

    setText(0, QString(oss.str().c_str()));
}

BPESTreeWidgetItem::~BPESTreeWidgetItem() {}

void BPESTreeWidgetItem::draw(QGLViewer *viewer) const
{
}
