#include "nestreewidgetitem.h"
#include <sstream>
#include <QGLViewer/qglviewer.h>
#include <CGAL/Random.h>
#include "ietreewidgetitem.h"
#include "cetreewidgetitem.h"

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

    pointSv.x = x;
    pointSv.y = y;
    pointSv.z = z;
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
    typedef NormalEventSite::Start_events_range StartEventsRange;
    typedef NormalEventSite::Start_events_iterator StartEventsIterator;
    StartEventsRange start_events(nes);
    for (StartEventsIterator it = start_events.begin();
         it != start_events.end(); it++)
    { addChild(new CETreeWidgetItem(*it, parent)); }

    // Add end events
    typedef NormalEventSite::End_events_range EndEventsRange;
    typedef NormalEventSite::End_events_iterator EndEventsIterator;
    EndEventsRange end_events(nes);
    for (EndEventsIterator it = end_events.begin();
         it != end_events.end(); it++)
    { addChild(new CETreeWidgetItem(*it, parent)); }
}

NESTreeWidgetItem::~NESTreeWidgetItem() {}

void NESTreeWidgetItem::draw(QGLViewer *viewer) const
{ pointSv.draw(viewer); }
