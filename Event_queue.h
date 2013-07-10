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

  Tagged_event(Tag_type t):
    tag(t) {}

  Tag_type tag;

  bool is_start() const
  { return tag == Start; }

  bool is_end() const
  { return tag == End; }

  bool operator==(const Tagged_event & ev) const
  { return tag == ev.tag; }
};

// Basic normal events are defined by:
//  - the pair of arcs defining its circle (pair of intersecting arcs ?)
//  - a tag { Start, End }
template <typename Kernel>
class Normal_event: public Tagged_event
{
  typedef typename Kernel::Circular_arc_3 Circular_arc_3;

  public:
    Normal_event(const Circular_arc_3 & ca1, const Circular_arc_3 & ca2,
        Tagged_event::Tag_type tag):
      Tagged_event(tag), arcs(ca1, ca2) {}

    std::pair<Circular_arc_3, Circular_arc_3> arcs;

    bool operator==(const Normal_event<Kernel> & ev) const
    { return Tagged_event::operator==(ev) && arcs == ev.arcs; }
};

// (Bi)Polar events are defined by:
//  - the circle itself (~arc bounded by poles)
//  - a tag { Start, End }
//
// Difference between polar and bipolar events
// is done by accessing the "polarity" flag.
template <typename Kernel>
class Polar_event: public Tagged_event
{
  typedef typename Kernel::Circle_3 Circle_3;
  typedef Sphere_intersecter<Kernel> SI;
  typedef typename SI::Circle_handle Circle_handle;

  public:
    enum Polarity_type {
      Single, Dual
    };

    enum Pole_type {
      North, South
    };

    Polar_event(const Circle_handle & ch,
        Polarity_type t, Pole_type p,
        Tagged_event::Tag_type tag):
      Tagged_event(tag), circle_handle(ch),
      polarity(t), pole(p) {}

    Circle_handle circle_handle;
    Polarity_type polarity;
    Pole_type pole;

    bool is_single_polar() const
    { return polarity == Single; }

    bool is_bipolar() const
    { return polarity == Dual; }

    bool is_north() const
    { return pole == North; }

    bool is_south() const
    { return pole == North; }

    const Circle_3 & circle() const
    { return *circle_handle; }

    bool operator==(const Polar_event<Kernel> & ev) const
    { return Tagged_event::operator==(ev)
      && circle_handle == ev.circle_handle
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
        Intersection_type t):
      circles(c1, c2), tag(t) {}

    bool operator==(const Intersection_event<Kernel> & ev) const
    { return circles == ev.circles && tag == ev.tag; }
};

// Compare two events' first arc by increasing radius,
// works with Normal/Intersection events.
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

  // Iterator ranges should have access to hidden members
  friend class Intersection_events_range;

  // Compare theta-z
  typedef Comp_theta_z_3<Kernel> Compare_theta_z_3;

  // Compare normal events' circles (increasing/decreasing)
  typedef Comp_event_arc_radii<Kernel, Normal_event<Kernel> > Comp_ne_arc_radii;
  typedef Comp_event_inv_arc_radii<Kernel, Normal_event<Kernel> > Comp_ne_inv_arc_radii;
  // Start/End events
  typedef std::multiset<Normal_event<Kernel>, Comp_ne_inv_arc_radii> End_normal_events;
  typedef std::multiset<Normal_event<Kernel>, Comp_ne_arc_radii> Start_normal_events;
  // Crossing/Tangency events
  typedef std::vector<Intersection_event<Kernel> > Intersection_events;

  public:
    Normal_event_site(const Sphere_handle & sh,
        const Circular_arc_point_3 & p):
      _point(p), _sphere_handle(sh), _start_normal_events(),
      _end_normal_events(), _intersection_events() {}

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
      if (ev.is_start()) { _start_normal_events.insert(ev); }
      else { _end_normal_events.insert(ev); }
    }

    // Overload for adding an intersection event
    void add_event(const Intersection_event<Kernel> & ev)
    { _intersection_events.push_back(ev); }

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
    { return Compare_theta_z_3()(_point, es._point, *_sphere_handle); }

    // Symmetric version of Polar_event_site::occurs_before
    bool occurs_before(const Polar_event_site<Kernel> & es) const
    { return es.occurs_before(*this) == false; }

    // Getter for event point
    Circular_arc_point_3 const & point() const
    { return _point; }

    // Start events iterator range
    // TODO

    // End events iterator range
    // TODO

    // Intersection events iterator range
    typedef typename Intersection_events::const_iterator
      Intersection_events_iterator;
    class Intersection_events_range
    {
      public:
        Intersection_events_range(const Normal_event_site<Kernel> & nes):
          _nes(nes) {}

        Intersection_events_iterator begin() const
        { return _nes._intersection_events.begin(); }

        Intersection_events_iterator end() const
        { return _nes._intersection_events.end(); }

      private:
        const Normal_event_site<Kernel> & _nes;
    };

    // Get the corresponding sphere handle
    const Sphere_handle & sphere_handle() const
    { return _sphere_handle; }

  private:
    // Location of event site
    Circular_arc_point_3 _point;
    // ...source sphere
    Sphere_handle _sphere_handle;

    // Start/End events
    Start_normal_events _start_normal_events;
    End_normal_events _end_normal_events;

    // Crossing/Tangency events
    Intersection_events _intersection_events;
};

template <typename Kernel>
class Polar_event_site
{
  typedef typename Kernel::Circle_3 Circle_3;

  public:
    Polar_event_site(const Polar_event<Kernel> & ev):
      _event(ev) {}

    bool is_single_polar() const
    { return _event.is_single_polar(); }

    bool is_bipolar() const
    { return _event.is_bipolar(); }

    bool occurs_before(const Normal_event_site<Kernel> &) const
    { return is_bipolar() || _event.is_end(); }

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
          if (Comp_by_squared_radii<Circle_3>()
              (es._event.circle(), _event.circle()))
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
    // Helper enum for returning either one of the type
    // of events that can be handled by the queue, or none
    enum Event_site_type {
      None, Normal, Polar
    };

    Event_queue():
      _normal_events(), _polar_events(),
      _computed_state(None) {}

    // Push events to our queue
    void push(const Normal_event_site<Kernel> & es)
    { _normal_events.push(es);
      update_state(); }
    void push(const Polar_event_site<Kernel> & es)
    { _polar_events.push(es);
      update_state(); }

    // Helper for checking if event queue is empty
    bool empty() const
    { return next_event() == None; }

    // Get the type of the next event to handle
    // Performance enhancement using state saving
    Event_site_type next_event() const
    { return _computed_state; }

    // Pop a polar event from the queue
    // Concept: there is at least one polar event
    Polar_event_site<Kernel> pop_polar()
    {
      Polar_event_site<Kernel> pes = _polar_events.top();
      _polar_events.pop();
      update_state();
      return pes;
    }

    // Get a polar event from the queue (without removing it)
    // Concept: there is at least one polar event
    const Polar_event_site<Kernel> & top_polar() const
    { return _polar_events.top(); }

    // Pop a normal event from the queue
    // Concept: there is at least one normal event
    Normal_event_site<Kernel> pop_normal()
    {
      Normal_event_site<Kernel> nes = _normal_events.top();
      _normal_events.pop();
      update_state();
      return nes;
    }

    // Get a normal event from the queue (without removing it)
    // Concept: there is at least one normal event
    const Normal_event_site<Kernel> & top_normal() const
    { return _normal_events.top(); }

  private:
    // Fire an update of the event queue
    void update_state()
    { _computed_state = compute_next_state(); }
    // ...compute the next state
    Event_site_type compute_next_state() const
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
      const Normal_event_site<Kernel> & nes = _normal_events.top();
      const Polar_event_site<Kernel> & pes = _polar_events.top();
      if (nes.occurs_before(pes))
      { return Normal; }
      else
      { return Polar; }
    }

    template <typename Event_site>
    struct Event_site_queue
    { typedef std::priority_queue<Event_site, std::vector<Event_site>,
      Comp_event_sites<Event_site> > Type; };

    // Normal event sites
    typename Event_site_queue<Normal_event_site<Kernel> >::Type _normal_events;

    // Polar event sites
    typename Event_site_queue<Polar_event_site<Kernel> >::Type _polar_events;

    // Event queue state, reset after each pop/push, computed
    Event_site_type _computed_state;
};

#endif // EVENT_QUEUE_H // vim: sw=2 et ts=2 sts=2
