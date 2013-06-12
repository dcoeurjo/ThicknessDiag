#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <queue>
#include <functional>
#include <event_sites.h>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

template <typename Kernel>
class Event_queue
{
  public:
    // Push events to our queue
    void push(const Normal_event_site<Kernel> & es)
    { _normal_events.push(es); }
    void push(const Polar_event_site<Kernel> & es)
    { _polar_events.push(es); }
    
    // Helper enum for returning either one of the type
    // of events that can be handled by the queue, or none
    enum Event_site_type {
      None, Normal, Polar
    };

    // Get the type of the next event to handle
    Event_site_type next_event() const
    {
      // Basic case: no events are left
      if (_normal_events.empty() && _polar_events.empty())
      { return None; }

      // Basic case for when only one of the queues is empty
      if (_polar_events.empty())
      { return Normal; }
      else
      { return Polar; }

      // General case for when both queues aren't empty
      if (_normal_events.top().occurs_before(_polar_events.top()))
      { return Normal; }
      else
      { return Polar; }
    }

    // Pop a polar event from the queue
    // Concept: there is at least one polar event
    Polar_event_site<Kernel> pop_polar_event()
    { return _polar_events.pop(); }

    // Pop a normal event from the queue
    // Concept: there is at least one normal event
    Normal_event_site<Kernel> pop_normal_event()
    { return _normal_events.pop(); }

  private:
    // Refactoring since events sites aren't polymorphic
    // but at least define an "occurs_before" method
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

    // Normal event sites
    typedef Comp_event_sites<Normal_event_site<Kernel> >
      Comp_normal_event_sites;
    std::priority_queue<Normal_event_site<Kernel>,
      Comp_normal_event_sites> _normal_events;

    // Polar event sites
    typedef Comp_event_sites<Polar_event_site<Kernel> >
      Comp_polar_event_sites;
    std::priority_queue<Polar_event_site<Kernel>,
      Comp_polar_event_sites> _polar_events;
};

#endif // EVENT_QUEUE_H // vim: sw=2 et ts=2 sts=2
