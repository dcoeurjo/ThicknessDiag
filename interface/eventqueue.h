#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <Event_queue.h>
#include <Event_queue_builder.h>

#include "kernel.h"

typedef Event_queue<Kernel> EventQueue;
typedef Event_queue_builder<Kernel> EventQueueBuilder;

#endif // EVENTQUEUE_H
