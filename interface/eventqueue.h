#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include "kernel.h"
#include <Event_queue.h>

typedef Event_queue<Kernel> EventQueue;
typedef EventQueue::Event_site_type EventSiteType;
typedef EventQueue::Events Events;

typedef Events::Critical_event CriticalEvent;
typedef Events::Intersection_event IntersectionEvent;
typedef Events::Normal_event_site NormalEventSite;

typedef Events::Polar_event PolarEvent;
typedef Events::Polar_event_site PolarEventSite;

#endif // EVENTQUEUE_H
