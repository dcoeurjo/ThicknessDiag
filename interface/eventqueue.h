#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include "kernel.h"
#include <Event_queue.h>

typedef Event_queue<Kernel> EventQueue;
typedef EventQueue::Event_site_type EventSiteType;

typedef Normal_event<Kernel> NormalEvent;
typedef Intersection_event<Kernel> IntersectionEvent;
typedef Normal_event_site<Kernel> NormalEventSite;

typedef Polar_event<Kernel> PolarEvent;
typedef Polar_event_site<Kernel> PolarEventSite;

#endif // EVENTQUEUE_H
