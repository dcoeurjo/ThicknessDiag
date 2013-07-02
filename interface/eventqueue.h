#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <Event_queue.h>
#include <Event_queue_builder.h>

#include "kernel.h"

// Event queue
typedef Event_queue<Kernel> EventQueue;
typedef Event_queue_builder<Kernel> EventQueueBuilder;

// Polar events
typedef Polar_event<Kernel> PolarEvent;
typedef Polar_event_site<Kernel> PolarEventSite;

// Normal events
typedef Normal_event<Kernel> NormalEvent;
typedef Intersection_event<Kernel> IntersectionEvent;
typedef Normal_event_site<Kernel> NormalEventSite;

#endif // EVENTQUEUE_H
