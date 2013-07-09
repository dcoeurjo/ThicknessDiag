#include "normaleventsitetreewidget.h"
#include <sstream>

NormalEventSiteTreeWidget::NormalEventSiteTreeWidget(const NormalEventSite &nes, QTreeWidget *parent):
    QTreeWidgetItem(parent), nes(nes)
{
    using CGAL::to_double;
    std::ostringstream oss;
    oss << "Normal event site [" << to_double(nes.point().x()) << ", "
        << to_double(nes.point().y()) << ", " << to_double(nes.point().z()) << "]"
        << ", theta = " << 180.*std::acos(to_double(nes.sphere_handle()->center().x() - nes.point().x())
                                     / std::sqrt(to_double(nes.sphere_handle()->squared_radius()))) / M_PI;
    setText(0, QString(oss.str().c_str()));
}

NormalEventSiteTreeWidget::~NormalEventSiteTreeWidget() {}