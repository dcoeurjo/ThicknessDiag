#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <set>
#include <queue>
#include <vector>
#include <functional>

template <typename K> class Event_site;

template <typename K>
class Event_queue
{
  public:
    Event_queue():
      _event_sites() {}

    Event_queue(const Event_queue & eq):
      _event_sites(eq._event_sites) {}

    ~Event_queue() {}

    Event_queue & operator=(const Event_queue & eq)
    {
      if (&eq != this)
      { _event_sites = eq._event_sites; }
      return *this;
    }

    // TODO implement event queue functionalities

  private:
    struct Compare_event_sites: std::unary_function<Event_site<K>, bool>
    {
      bool operator()(const Event_site<K> &, const Event_site<K> &) const;
    };

    std::priority_queue<Event_site<K>,
      std::vector<Event_site<K> >, Compare_event_sites> _event_sites;
};

template <typename K>
class Event_site
{
  public:
    typedef typename K::Point_3 Point;
    typedef typename K::Circle_3 Circle;

    Event_site(const Point & p):
      _point(p), _start_events(),
      _end_events(), _ct_events() {}

    Event_site(const Event_site & es):
      _point(es._point), _start_events(es._start_events),
      _end_events(es._end_events), _ct_events(es._ct_events) {}

    ~Event_site() {}

    Event_site & operator=(const Event_site & es)
    {
      if (&es != this)
      {
        _point = es._point;
        _start_events = es._start_events;
        _end_events = es._end_events;
        _ct_events = es._ct_events;
      }
      return *this;
    }

    const Point & point() const
    { return _point; }

    // Tag to pass when adding an event to the site
    enum Event_tag {
      Start, End,
      Crossing, Tangency
    };

    // Add an event to the site, passing the event's semantic
    void add_event(const Circle & c, Event_tag t)
    {
      // TODO
    }

  private:
    // Point of the event site
    Point _point;

    // Compare circles by increasing radius
    struct Compare_circle_radii: std::unary_function<Circle, bool>
    {
      bool operator()(const Circle & left,
          const Circle & right) const
      {
        return left.squared_radius() < right.squared_radius();
      }
    };

    // Compare circles by decreasing radius
    typedef std::unary_negate<Compare_circle_radii>
      Compare_inv_circle_radii;

    // Start events (first point where meridian crosses circle)
    std::multiset<Circle, Compare_circle_radii> _start_events;
    // End events (second point where meridian crosses circle)
    std::multiset<Circle, Compare_inv_circle_radii> _end_events;

    // Crossing/Tangency events
    std::vector<std::pair<Event_tag, Circle> > _ct_events;
};

#endif // EVENT_QUEUE_H // vim: sw=2 et ts=2 sts=2 tw=2
