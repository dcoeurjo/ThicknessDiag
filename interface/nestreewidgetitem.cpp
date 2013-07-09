#include "nestreewidgetitem.h"

#include <sstream>

#include <QGLViewer/qglviewer.h>

#include <CGAL/Random.h>

static CGAL::Random randgen;

NESTreeWidgetItem::NESTreeWidgetItem(const NormalEventSite &nes, QTreeWidget *parent):
    ESTreeWidgetItem(parent), nes(nes)
{
    using CGAL::to_double;
    std::ostringstream oss;
    double sphere_radius = std::sqrt(to_double(nes.sphere_handle()->squared_radius()));
    double x = to_double(nes.point().x());
    double y = to_double(nes.point().y());
    double z = to_double(nes.point().z());
    oss << "Normal event site [" << x << ", "
        << y << ", " << z << "]"
        << ", theta = " << 180.*std::acos(to_double(nes.sphere_handle()->center().x() - x) / sphere_radius) / M_PI;
    setText(0, QString(oss.str().c_str()));

    pointSv.frame.setPosition(x, y, z);
    pointSv.radius = sphere_radius / 100.;
    pointSv.color.setRed(randgen.get_int(0, 255));
    pointSv.color.setGreen(randgen.get_int(0, 255));
    pointSv.color.setBlue(randgen.get_int(0, 255));
}

NESTreeWidgetItem::~NESTreeWidgetItem() {}

void NESTreeWidgetItem::draw(QGLViewer *viewer)
{
    pointSv.draw(viewer);
}
