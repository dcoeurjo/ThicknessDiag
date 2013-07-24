#ifndef EVENT_QUEUE_BUILDER_H
#define EVENT_QUEUE_BUILDER_H

#include <Event_queue.h>
#include <Sphere_intersecter.h>

template <typename SK>
struct Event_queue_builder
{
  Event_queue<SK> operator()(const Sphere_intersecter<SK> &,
      typename SK::Sphere_3 const &);
  Event_queue<SK> operator()(const Sphere_intersecter<SK> &,
      typename Sphere_intersecter<SK>::Sphere_handle const &);
};

#endif // EVENT_QUEUE_BUILDER_H // vim: sw=2 et ts=2 sts=2
