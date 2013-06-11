#ifndef EVENT_SITES_H
#define EVENT_SITES_H

#include <set>
#include <vector>
#include <functional>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

#include <events.h>

template <typename K>
class Polar_event_site;

template <typename K>
class Normal_event_site
{
  public:
    Normal_event_site(typename K::Point_3 const & p):
      _point(p), _starts(),
      _ends(), _cts() {}

    void add_event(const Normal_event<K> & ev)
    {
#ifndef NDEBUG // Check that events added to the site are *valid*
      typename K::Circle_3 c(ev.arcs.first.supporting_circle()),
               ac(ev.arcs.second.supporting_circle());

      // Both arcs of event should lie on the same circle
      if (c != ac)
      {
        std::ostringstream oss;
        oss << "Normal event isn't valid, arcs do not lie on the "
          << "same circle " << c << " != " << ac;
        throw std::logic_error(oss.str());
      }

      // Event site should be located somewhere on the event's arcs
      if (c.has_on(_point) == false)
      {
        std::ostringstream oss;
        oss << "Can only add valid events to normal event site at "
          << _point << ", circle " << c << " does not pass here";
        throw std::logic_error(oss.str());
      }
#endif // NDEBUG //

      // Add event to accurate container
      if (ev.is_start()) { _starts.insert(ev); }
      else { _ends.insert(ev); }
    }

    void add_event(const Intersection_event<K> & ev)
    {
      _cts.push_back(ev);
    }

    // TODO must be modified to follow CGAL's concepts
    //  for now this simply indicates the rules to follow,
    //  and probably doesn't actually work
    bool occurs_before(const Normal_event_site<K> & es) const
    {
      return _point.theta < es._point.theta
        || (_point.theta == es._point.theta
            && _point.z > es._point.z);
    }

    bool occurs_before(const Polar_event_site<K> & es) const
    {
      return es.is_single_polar() && es.is_start();
    }

  private:
    // Location of event site
    typename K::Point_3 _point;

    // Compare circles by increasing radius
    struct Comp_event_arc_radii: std::unary_function<Normal_event<K>, bool>
    {
      bool operator()(const Normal_event<K> & left,
          const Normal_event<K> & right) const
      {
        return left.arcs.first.squared_radius() < right.arcs.first.squared_radius();
      }
    };

    // Compare circles by decreasing radius
    typedef std::unary_negate<Comp_event_arc_radii>
      Comp_inv_event_arc_radii;

    // Start/End events
    std::multiset<Normal_event<K>, Comp_event_arc_radii> _starts;
    std::multiset<Normal_event<K>, Comp_inv_event_arc_radii> _ends;

    // Crossing/Tangency events
    std::vector<Intersection_event<K> > _cts;
};

template <typename K>
class Polar_event_site
{
  public:
    Polar_event_site(const Polar_event<K> & ev):
      _event(ev) {}

    bool is_single_polar() const
    { return _event.is_single_polar(); }

    bool is_bipolar() const
    { return _event.is_bipolar(); }

    bool occurs_before(const Normal_event_site<K> & es) const
    {
      return is_bipolar() || _event.is_end();
    }

    bool occurs_before(const Polar_event_site<K> & es) const
    {
      if (_event.polarity != es._event.polarity)
      {
        if (_event.is_single_polar())
        { return _event.is_end(); } // Polar end before bipolar
        else
        { return es._event.is_start(); } // Bipolar only before polar start
      }
      else if (_event.is_single_polar())
      {
        if (_event.tag != es._event.tag)
        { return _event.is_end(); } // End before start
        else if (_event.pole != es._event.pole)
        { return _event.is_north(); } // North pole before south
        else
        {
          // Store the polar event site having the biggest
          // associated circle radius
          const Polar_event_site<K> & es_biggest_circle = *this;
          if (es._event.arc.squared_radius() > _event.arc.squared_radius())
          { es_biggest_circle = es; }

          // Start -> biggest occurs first, end -> smallest occurs first
          if (_event.is_start())
          { return es_biggest_circle == *this; }
          else
          { return es_biggest_circle != *this; }
        }
      }
      else
      {
        return false; // FIXME two bipolar events cannot be in conflict (?)
      }
    }

  private:
    Polar_event<K> _event;
};

#endif // EVENT_SITES_H // vim: sw=2 et ts=2 sts=2 tw=2
