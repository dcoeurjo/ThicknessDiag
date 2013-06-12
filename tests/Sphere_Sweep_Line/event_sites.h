#ifndef EVENT_SITES_H
#define EVENT_SITES_H

#include <set>
#include <vector>
#include <utility>
#include <functional>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

// An event point is a point of the algorithm's base sphere
// where something relevant for the sweep process would occur.
// This notably includes:
//  1) tangency between the sweep meridian and a circle -> *critical* points
//  2) intersection between two arcs lying on the sphere (which
//      were created by the intersection of two other spheres
//      on the base sphere -> *crossing* points
//  3) tangency between two arcs lying on the sphere -> *tangency* points
//
// When cases (1) and either (2) or (3) are combined, we speak of
// *degenerate* points (crossing or tangency).

// Refactoring for event structures needing a mark
// for the { Start, End } tag.
//
// Current solution is a enum.
template <typename Kernel>
struct Tagged_event
{
  enum Tag_type {
    Start, End
  };

  Tag_type tag;

  bool is_start() const
  { return tag == Start; }

  bool is_end() const
  { return tag == End; }

  bool operator==(const Tagged_event<Kernel> & ev) const
  { return tag == ev.tag; }
};

// Basic normal events are defined by:
//  - the pair of arcs defining its circle 
//  - a tag { Start, End }
template <typename Kernel>
struct Normal_event: Tagged_event<Kernel>
{
  std::pair<typename Kernel::Circular_arc_3,
    typename Kernel::Circular_arc_3> arcs;

  bool operator==(const Normal_event<Kernel> & ev) const
  {
    return Tagged_event<Kernel>::operator==(ev)
      && arcs == ev.args;
  }
};

// Polar events are defined by:
//  - the arc looping around its entire circle
//  - a tag { Start, End }
//
// Bipolar events are defined by:
//  - the arc on the circle, bounded by the poles
//  - a tag { Start, End }
//
// Both are regrouped under one single struct because
// of the very similar structure of these two. Difference
// between these is done by accessing the polarity flag.
template <typename Kernel>
struct Polar_event: Tagged_event<Kernel>
{
  typename Kernel::Circular_arc_3 arc;

  enum Polarity_type {
    Single, Dual
  };

  Polarity_type polarity;

  bool is_single_polar() const
  { return polarity == Single; }

  bool is_bipolar() const
  { return polarity == Dual; }

  enum Pole_type {
    North, South
  };

  Pole_type pole;

  bool is_north() const
  { return pole == North; }

  bool is_south() const
  { return pole == North; }

  bool operator==(const Polar_event<Kernel> & ev) const
  {
    return Tagged_event<Kernel>::operator==(ev)
      && arc == ev.arc
      && polarity == ev.polarity
      && pole == ev.pole;
  }
};

// Intersection normal events are defined by:
//  - the intersection point (implicit ?)
//  - the pair of intersecting arcs
//  - a tag { Smallest_Crossing, Largest_Crossing, Tangency }
//
// Note: points are compared using lexicographic
//  order away from poles. Treated as a normal event.
template <typename Kernel>
struct Intersection_event
{
  std::pair<typename Kernel::Circular_arc_3,
    typename Kernel::Circular_arc_3> arcs;

  enum Intersection_type {
    Smallest_Crossing,
    Largest_Crossing,
    Tangency
  };

  Intersection_type tag;

  bool operator==(const Intersection_event<Kernel> & ev) const
  { return arcs == ev.arcs && tag == ev.tag; }
};

// An event site is the location where several events come in
// conflict, and allows ordering independently of the type of
// location, but more on the type of the underlying event.

template <typename Kernel>
class Polar_event_site;

template <typename Kernel>
class Normal_event_site
{
  public:
    Normal_event_site(typename Kernel::Point_3 const & p):
      _point(p), _starts(),
      _ends(), _cts() {}

    // Add a base normal event
    void add_event(const Normal_event<Kernel> & ev)
    {
#ifndef NDEBUG // Check that events added to the site are *valid*
      typename Kernel::Circle_3 c(ev.arcs.first.supporting_circle()),
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

    // Overload for adding an intersection event
    void add_event(const Intersection_event<Kernel> & ev)
    { _cts.push_back(ev); }

    // Range version of add_event taking any iterator
    // verifying the STL InputIterator concept
    template <typename InputIterator>
    void add_event(InputIterator begin, InputIterator end)
    {
      for (; begin != end; begin++)
      { add_event(*begin); }
    }

    // TODO must be modified to follow CGAL's concepts
    //  for now this simply indicates the rules to follow,
    //  and probably doesn't actually work
    //bool occurs_before(const Normal_event_site<Kernel> & es) const
    //{
    //  return _point.theta < es._point.theta
    //    || (_point.theta == es._point.theta
    //        && _point.z > es._point.z);
    //}
    //
    // Implemented version, using lexicographic comparing. This introduces
    // the concepts that points are compared lexicographically and are
    // placed in a frame local to the sphere (ie with its origin at the
    // center of the sphere), in cylindrical coordinates.
    bool occurs_before(const Normal_event_site<Kernel> & es) const
    {
      typename Kernel::CompareThetaZ_3 comp;
      typename Kernel::Circular_arc_point_3 cp(_point), es_cp(es._point);
      return comp.fo(cp, es_cp);
    }

    bool occurs_before(const Polar_event_site<Kernel> & es) const
    { return es.occurs_before(*this) == false; }

  private:
    // Location of event site
    typename Kernel::Point_3 _point;

    // Compare circles by increasing radius
    struct Comp_event_arc_radii:
      std::unary_function<Normal_event<Kernel>, bool>
    {
      bool operator()(const Normal_event<Kernel> & left,
          const Normal_event<Kernel> & right) const
      {
        typedef typename Kernel::Circular_arc_3 Arc;
        const Arc & left_arc = left.arcs.first,
              & right_arc = right.arcs.first;
        return left_arc.squared_radius() < right_arc.squared_radius();
      }
    };

    // Compare circles by decreasing radius
    typedef std::unary_negate<Comp_event_arc_radii>
      Comp_inv_event_arc_radii;

    // Start/End events
    std::multiset<Normal_event<Kernel>, Comp_event_arc_radii> _starts;
    std::multiset<Normal_event<Kernel>, Comp_inv_event_arc_radii> _ends;

    // Crossing/Tangency events
    std::vector<Intersection_event<Kernel> > _cts;
};

template <typename Kernel>
class Polar_event_site
{
  public:
    Polar_event_site(const Polar_event<Kernel> & ev):
      _event(ev) {}

    bool is_single_polar() const
    { return _event.is_single_polar(); }

    bool is_bipolar() const
    { return _event.is_bipolar(); }

    bool occurs_before(const Normal_event_site<Kernel> & es) const
    {
      return is_bipolar() || _event.is_end();
    }

    bool occurs_before(const Polar_event_site<Kernel> & es) const
    {
      if (_event.polarity != es._event.polarity) // Different polarities
      {
        if (_event.is_single_polar())
        { return _event.is_end(); } // Polar end before bipolar
        else
        { return es._event.is_start(); } // Bipolar only before polar start
      }
      else if (_event.is_single_polar()) // Both "single" polar
      {
        if (_event.tag != es._event.tag)
        { return _event.is_end(); } // End before start
        else if (_event.pole != es._event.pole)
        { return _event.is_north(); } // North pole before south
        else
        {
          // Store the polar event site having the biggest
          // associated circle radius
          const Polar_event_site<Kernel> & es_biggest_circle = *this;
          if (es._event.arc.squared_radius() > _event.arc.squared_radius())
          { es_biggest_circle = es; }

          // Start -> biggest occurs first, end -> smallest occurs first
          if (_event.is_start())
          { return es_biggest_circle == *this; }
          else
          { return es_biggest_circle != *this; }
        }
      }
      else // Both bipolar
      {
        return false; // FIXME two bipolar events cannot be in conflict (?)
      }
    }

  private:
    Polar_event<Kernel> _event;
};

#endif // EVENT_SITES_H // vim: sw=2 et ts=2 sts=2
