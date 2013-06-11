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
      if (ev.is_start) { _starts.insert(ev); }
      else { _ends.insert(ev); }
    }

    void add_event(const Intersection_event<K> & ev)
    {
      _cts.push_back(ev);
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

#endif // EVENT_SITES_H // vim: sw=2 et ts=2 sts=2 tw=2
