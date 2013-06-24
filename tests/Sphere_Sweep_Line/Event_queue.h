#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <set>
#include <queue>
#include <vector>
#include <utility>
#include <functional>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

#include <Spherical_utils.h>
#include <Sphere_intersecter.h>

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

  bool operator==(const Tagged_event & ev) const
  { return tag == ev.tag; }
};

// Basic normal events are defined by:
//  - the pair of arcs defining its circle 
//  - a tag { Start, End }
template <typename Kernel>
class Normal_event: public Tagged_event
{
  typedef typename Kernel::Circular_arc_3 Circular_arc_3;

  public:
    std::pair<Circular_arc_3, Circular_arc_3> arcs;

    bool operator==(const Normal_event<Kernel> & ev) const
    { return Tagged_event::operator==(ev) && arcs == ev.arcs; }
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
class Polar_event: public Tagged_event
{
  typedef typename Kernel::Circular_arc_3 Circular_arc_3;

  public:
    enum Polarity_type {
      Single, Dual
    };

    Polarity_type polarity;
    Circular_arc_3 arc;

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
    { return Tagged_event::operator==(ev) && arc == ev.arc
      && polarity == ev.polarity && pole == ev.pole; }
};

// Intersection normal events are defined by:
//  - the intersection point (implicit, but stored for convenience)
//  - the pair of intersecting arcs (<-> circles)
//  - a tag { Smallest_crossing, Largest_crossing, Tangency }
//
// Note: points are compared using lexicographic
//  order away from poles. Treated as a normal event.
template <typename Kernel>
class Intersection_event
{
  typedef typename Kernel::Circle_3 Circle_3;

  public:
    enum Intersection_type {
      Smallest_crossing,
      Largest_crossing,
      Tangency
    };

    std::pair<Circle_3, Circle_3> circles;
    Intersection_type tag;

    Intersection_event(const Circle_3 & c1, const Circle_3 & c2,
        const Intersection_type & t):
      circles(c1, c2), tag(t) {}

    bool operator==(const Intersection_event<Kernel> & ev) const
    { return circles == ev.circles && tag == ev.tag; }
};

// Compare two events' first arc by increasing radius
template <typename Kernel, typename Event>
struct Comp_event_arc_radii:
  std::unary_function<Event, bool>
{
  bool operator()(const Event & left,
      const Event & right) const
  {
    typedef typename Kernel::Circular_arc_3 Arc_type;
    return Comp_by_squared_radii<Arc_type>()(left.arcs.first, right.arcs.first);
  }
};
// ...inverse
template <typename Kernel, typename Event>
struct Comp_event_inv_arc_radii:
  std::unary_function<Event, bool>
{ bool operator()(const Event & left,
      const Event & right) const
  { return Comp_event_arc_radii<Kernel, Event>()(right, left); } };

// An event site is the location where several events come in
// conflict, and allows ordering independently of the type of
// location, but more on the type of the underlying event.

template <typename Kernel>
class Polar_event_site;

template <typename Kernel>
class Normal_event_site
{
  typedef typename Kernel::Circle_3 Circle_3;
  typedef typename Kernel::Circular_arc_point_3 Circular_arc_point_3;

  typedef Sphere_intersecter<Kernel> SI;
  typedef typename SI::Sphere_handle Sphere_handle;

  public:
    Normal_event_site(const Sphere_handle & sh,
        const Circular_arc_point_3 & p):
      _sphere_handle(sh), _point(p), _starts(),
      _ends(), _cts() {}

    // Add a base normal event
    void add_event(const Normal_event<Kernel> & ev)
    {
#ifndef NDEBUG // Check that events added to the site are *valid*
      Circle_3 c(ev.arcs.first.supporting_circle()),
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
    { for (; begin != end; begin++)
      { add_event(*begin); } }

    // Done using lexicographic comparing. This introduces
    // the concepts that points are compared lexicographically and are
    // placed in a frame local to the sphere (ie with its origin at the
    // center of the sphere), in cylindrical coordinates.
    bool occurs_before(const Normal_event_site<Kernel> & es) const
    { return Comp_theta_z_3<Kernel>()(_point, es._point, *_sphere_handle); }

    // Symmetric version of Polar_event_site::occurs_before
    bool occurs_before(const Polar_event<Kernel> & es) const
    { return es.occurs_before(*this) == false; }

    // Getter for event point
    Circular_arc_point_3 const & point() const
    { return _point; }

  private:
    // Location of event site
    Circular_arc_point_3 _point;
    // ...source sphere
    Sphere_handle _sphere_handle;

    // Compare normal events' circles (increasing/decreasing)
    typedef Comp_event_arc_radii<Kernel, Normal_event<Kernel> > Comp_ne_arc_radii;
    typedef Comp_event_inv_arc_radii<Kernel, Normal_event<Kernel> > Comp_ne_inv_arc_radii;

    // Start/End events
    std::multiset<Normal_event<Kernel>, Comp_ne_arc_radii> _starts;
    std::multiset<Normal_event<Kernel>, Comp_ne_inv_arc_radii> _ends;

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

    bool occurs_before(const Normal_event<Kernel> & es) const
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
          const Polar_event_site<Kernel> * es_biggest_circle = this;
          if (es._event.arc.squared_radius() > _event.arc.squared_radius())
          { es_biggest_circle = &es; }

          // Start -> biggest occurs first, end -> smallest occurs first
          if (_event.is_start())
          { return es_biggest_circle == this; }
          else
          { return !(es_biggest_circle == this); }
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
struct Comp_event_sites:
  std::unary_function<bool, Event_site>
{
  bool operator()(const Event_site & left,
      const Event_site & right) const
  {
    return right.occurs_before(left);
  }
};

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
    template <typename Event_site>
    struct Event_site_queue
    { typedef std::priority_queue<Event_site, std::vector<Event_site>,
      Comp_event_sites<Event_site> > Type; };

    // Normal event sites
    typename Event_site_queue<Normal_event_site<Kernel> >::Type _normal_events;

    // Polar event sites
    typename Event_site_queue<Polar_event_site<Kernel> >::Type _polar_events;
};

#endif // EVENT_QUEUE_H // vim: sw=2 et ts=2 sts=2
