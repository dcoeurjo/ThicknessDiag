#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <queue>
#include <functional>
#include <event_sites.h>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

template <typename K>
struct Event_queue
{
  // Refactoring since events sites aren't polymorphic
  // but at least define a "occurs_before" method
  //
  // Event sites are denoted as "less than" when they are of
  // lower priority, that is if the other event site occurs
  // before the one we are working with. This garantees that
  // ordering event sites in a priority_queue this way will
  // always give us the most prioritary event via the queue's
  // "top" method.
  template <typename Event_site>
  struct Comp_event_sites: std::unary_function<bool, Event_site>
  {
    bool operator()(const Event_site & left,
        const Event_site & right) const
    {
      return right.occurs_before(left);
    }
  };

  // Helper enum for returning either one of the type
  // of events that can be handled by the queue
  enum Event_site_type {
    Normal, Polar
  };

  // Check if our event queue is empty (ie doesn't have a next event)
  bool has_next_event() const
  {
    return normal_events.empty() == false
      || polar_events.empty() == false;
  }

  // Get the type of the next event to handle
  //
  // Concept: there is a next event
  Event_site_type next_event() const
  {
#ifndef NDEBUG // Assertion check
    if (has_next_event() == false)
    {
      std::ostringstream oss;
      oss << "Concept violation: there must be a 'next event' "
        << "in order to get that next event's type";
      throw std::logic_error(oss.str());
    }
#endif

    // Basic case for when one of the queues is empty
    if (polar_events.empty())
    { return Normal; }
    else
    { return Polar; }

    // General case for when both queues aren't empty
    if (normal_events.top().occurs_before(polar_events.top()))
    { return Normal; }
    else
    { return Polar; }
  }

  // Normal event sites
  typedef Comp_event_sites<Normal_event_site<K> >
    Comp_normal_event_sites;
  std::priority_queue<Normal_event_site<K>,
    Comp_normal_event_sites> normal_events;

  // Polar event sites
  typedef Comp_event_sites<Polar_event_site<K> >
    Comp_polar_event_sites;
  std::priority_queue<Polar_event_site<K>,
    Comp_polar_event_sites> polar_events;
};

#endif // EVENT_QUEUE_H // vim: sw=2 et ts=2 sts=2
