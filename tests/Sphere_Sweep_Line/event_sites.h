#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <stdexcept>
#include <sstream>
#include <set>
#include <vector>
#include <functional>

template <typename K>
class Event_site
{
  public:
    typedef typename K::Point_3 Point;
    typedef typename K::Circle_3 Circle;

    Event_site(const Point & p):
      _point(p) {}

    const Point & point() const
    { return _point; }

  private:
    Point _point;
};

template <typename K>
class Polar_event_site: public Event_site<K>
{
  public:
    Polar_event_site(typename Event_site<K>::Point const & p):
      Event_site<K>(p) {}
};

template <typename K>
class Bipolar_event_site: public Event_site<K>
{
  public:
    Bipolar_event_site(typename Event_site<K>::Point const & p):
      Event_site<K>(p) {}
};

template <typename K>
class Normal_event_site: public Event_site<K>
{
  public:
    typedef typename Event_site<K>::Point Point;
    typedef typename Event_site<K>::Circle Circle;

    Normal_event_site(const Point & p):
      Event_site<K>(p), _starts(),
      _ends(), _cts() {}

    // Tag to pass when adding an event to the site
    enum Event_tag {
      Start, End,
      Crossing, Tangency
    };

    // Add an event to the site, passing the event's semantic
    void add_event(const Circle & c, Event_tag t)
    {
      // Only valid events can be added to the site
      const Point & p = Event_site<K>::point();
      if (c.has_on(p) == false)
      {
        std::ostringstream oss;
        oss << "Can only add valid events to normal event site at "
          << p << ", circle " << c << " does not pass here";
        throw std::logic_error(oss.str());
      }

      // Add event to accurate container
      switch (t)
      {
        case Start:
          _starts.insert(c);

        case End:
          _ends.insert(c);

        case Crossing:
        case Tangency:
          _cts.insert(c);
      }
    }

  private:
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

    // Start/End events
    std::multiset<Circle, Compare_circle_radii> _starts;
    std::multiset<Circle, Compare_inv_circle_radii> _ends;

    // Crossing/Tangency events
    std::vector<std::pair<Event_tag, Circle> > _cts;
};

#endif // EVENT_QUEUE_H // vim: sw=2 et ts=2 sts=2 tw=2
