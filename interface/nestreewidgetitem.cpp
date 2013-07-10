#include "nestreewidgetitem.h"

#include <sstream>

#include <QGLViewer/qglviewer.h>

#include <CGAL/Random.h>

static CGAL::Random randgen;

NESTreeWidgetItem::NESTreeWidgetItem(const NormalEventSite &nes, QTreeWidget *parent):
    ESTreeWidgetItem(parent), nes(nes)
{
    // Convert (x, y, z) coordinates and compute theta angle
    using CGAL::to_double;
    double sphere_radius = std::sqrt(to_double(nes.sphere_handle()->squared_radius()));
    double x = to_double(nes.point().x());
    double y = to_double(nes.point().y());
    double z = to_double(nes.point().z());
    double theta = std::atan2(to_double(nes.sphere_handle()->center().x() - nes.point().x()),
                              to_double(nes.sphere_handle()->center().y() - nes.point().y()));
    //theta += M_PI;
    theta *= static_cast<double>(180) / M_PI;

    // Set label text
    std::ostringstream oss;
    oss << "Normal event site [" << x << ", "
        << y << ", " << z << "]"
        << ", theta = " << theta;
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
