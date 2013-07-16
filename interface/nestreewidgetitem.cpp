#include "nestreewidgetitem.h"

#include <sstream>

#include <QGLViewer/qglviewer.h>

#include <CGAL/Random.h>

#include "ietreewidgetitem.h"

static CGAL::Random randgen;

NESTreeWidgetItem::NESTreeWidgetItem(const NormalEventSite &nes, QTreeWidget *parent):
    DrawableTreeWidgetItem(parent), nes(nes)
{
    // Convert (x, y, z) coordinates and compute theta angle
    using CGAL::to_double;
    double sphere_radius = std::sqrt(to_double(nes.sphere()->squared_radius()));
    double x = to_double(nes.point().x());
    double y = to_double(nes.point().y());
    double z = to_double(nes.point().z());
    double theta = std::atan2(to_double(nes.sphere()->center().x() - nes.point().x()),
                              to_double(nes.sphere()->center().y() - nes.point().y()));
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

    // Add intersection events
    typedef NormalEventSite::Intersection_events_range IntersectionEventsRange;
    typedef NormalEventSite::Intersection_events_iterator IntersectionEventsIterator;
    IntersectionEventsRange intersection_events(nes);
    for (IntersectionEventsIterator it = intersection_events.begin();
         it != intersection_events.end(); it++)
    { addChild(new IETreeWidgetItem(*it, parent)); }

    // Add start events
    // Add end events
}

NESTreeWidgetItem::~NESTreeWidgetItem() {}

void NESTreeWidgetItem::draw(QGLViewer *viewer) const
{ pointSv.draw(viewer); }
