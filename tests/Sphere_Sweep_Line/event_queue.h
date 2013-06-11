#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <queue>
#include <event_sites.h>

template <typename K>
class Event_queue
{
  public:
    Event_queue():
      _normal_events(),
      _polar_events() {}

  private:
    template <typename Event_site>
    struct Comp_event_sites
    {
      bool operator()(const Event_site & left,
          const Event_site & right) const
      {
        return left.occurs_before(right);
      }
    };

    typedef Comp_event_sites<Normal_event_site<K> >
      Comp_normal_event_sites;
    typedef Comp_event_sites<Polar_event_site<K> >
      Comp_polar_event_sites;

    // Normal event sites
    std::priority_queue<Normal_event_site<K>,
      Comp_normal_event_sites> _normal_events;

    // Polar event sites
    std::priority_queue<Polar_event_site<K>,
      Comp_polar_event_sites> _polar_events;
};

#endif // EVENT_QUEUE_H // vim: sw=2 et ts=2 sts=2
