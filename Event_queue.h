#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <set>
#include <queue>
#include <vector>
#include <utility>

#include <CGAL/assertions.h>

#include <Spherical_utils.h>
#include <Sphere_intersecter.h>

// An event point is a point of the algorithm's base sphere
// where something relevant for the sweep process would occur.
// This notably includes:
//  1) tangency between the sweep meridian and a circle -> *critical* events
//  2) intersection between two arcs lying on the sphere (which
//      were created by the intersection of two other spheres
//      on the base sphere -> *crossing* events
//  3) tangency between two arcs lying on the sphere -> *tangency* events
//
// When cases (1) and either (2) or (3) are combined, we speak of
// *degenerate* points (crossing or tangency).

// "Templated namespace" reduces template headers for all of our
// event classes under this pseudo-namespace
template <typename SK>
class Event_bundle
{
  // Sphere intersecter
  typedef Sphere_intersecter<SK> SI;
  typedef typename SI::Circle_handle Circle_handle;
  typedef typename SI::Sphere_handle Sphere_handle;

  // Geometric objects
  typedef typename SK::Circle_3 Circle_3;
  typedef typename SK::Vector_3 Vector_3;
  typedef typename SK::Sphere_3 Sphere_3;
  typedef typename SK::Circular_arc_point_3 Circular_arc_point_3;

  // Function objects
  typedef typename SK::Compare_theta_3 Compare_theta_3;
  typedef typename SK::Compare_theta_z_3 Compare_theta_z_3;

  public:
    // Base class for all events, holding a link to the source sphere
    struct Sphere_event
    {
      Sphere_handle sphere;

      bool operator==(const Sphere_event & ev) const
      { return sphere == ev.sphere; }
    };

    // Used to refactor comparing between events which
    // are linked to a single circle as source. Also
    // regroups the start/end semantic for events, since
    // events on a single circle are necessarily start/end.
    struct Circle_event: Sphere_event
    {
      enum Tag_type {
        Start, End
      };

      Circle_handle circle;
      Tag_type tag;

      bool is_start() const
      { return tag == Start; }

      bool is_end() const
      { return tag == End; }

      bool operator==(const Circle_event & ev) const
      { return tag == ev.tag && circle == ev.circle; }
    };

    // Critical normal events are defined by:
    struct Critical_event: Circle_event
    {
      Circular_arc_point_3 point;

      bool operator==(const Critical_event & ev) const
      { return Circle_event::operator==(ev) && point == ev.point; }
    };

    // Intersection normal events are defined by:
    //  - a tag { Smallest_crossing, Largest_crossing, Tangency }
    //  - the pair of circles intersecting
    struct Intersection_event: Sphere_event
    {
      typedef std::pair<Circle_handle, Circle_handle> Circle_handle_pair;

      enum Intersection_type {
        Smallest_crossing,
        Largest_crossing,
        Tangency
      };

      Intersection_type type;
      Circle_handle_pair circles;
      Circular_arc_point_3 point;

      bool operator==(const Intersection_event & ev) const
      { return type == ev.type && circles == ev.circles && point == ev.point; }
    };

    // Polar events are defined by:
    //  - a pole { North, South }
    //  - a critical event's data
    struct Polar_event: Critical_event
    {
      enum Pole_type {
        North, South
      };

      Pole_type pole;

      bool is_north() const
      { return pole == North; }

      bool is_south() const
      { return pole == South; }

      bool operator==(const Polar_event & ev) const
      { return Critical_event::operator==(ev) && pole == ev.pole; }
    };

    // Bipolar events are defined by:
    //  - a normal vector to the plane containing the meridian at their given
    //    angle, also oriented in the positive direction of the meridian
    struct Bipolar_event: Circle_event
    {
      Vector_3 normal;
    };

    // Compare two circles events by their circles' radius (increasing)
    struct Comp_event_circle_radii: std::unary_function<Circle_event, bool>
    {
      bool operator()(const Circle_event & left,
          const Circle_event & right) const
      { return Comp_by_squared_radii<Circle_3>()
          (*left.circle, *right.circle); }
    };
    // ...inverse
    struct Comp_event_inv_circle_radii: std::unary_function<Circle_event, bool>
    {
      bool operator()(const Circle_event & left,
          const Circle_event & right) const
      { return Comp_event_circle_radii()(right, left); }
    };

    // Builders for events

    class Circle_event_builder;

    class Event_builder
    {
      public:
        Event_builder(const Sphere_handle & s):
          _sphere(s) {}

        // Setup a new builder destined to create events which are
        // linked to a single circle (Circle_event), passing the circle
        Circle_event_builder prepare_circle_event(const Circle_handle & c) const
        { return Circle_event_builder(c, _sphere); }

        // Build an intersection event, passing the
        // two circles in intersection
        Intersection_event intersection_event(const Circle_handle & first, const Circle_handle & second,
            const Circular_arc_point_3 & point, typename Intersection_event::Intersection_type type) const
        {
          Intersection_event ie;
          link_to_sphere(ie);
          typedef typename Intersection_event::Circle_handle_pair Circle_handle_pair;
          ie.circles = Circle_handle_pair(first, second);
          ie.type = type;
          ie.point = point;
          return ie;
        }

      protected:
        // Helper building method, setting the sphere member for events
        void link_to_sphere(Sphere_event & ev) const
        { ev.sphere = _sphere; }

      private:
        Sphere_handle _sphere;
    };

    class Circle_event_builder: public Event_builder
    {
      public:
        Circle_event_builder(const Circle_handle & c, const Sphere_handle & s):
          Event_builder(s), _circle(c) {}

        Critical_event critical_event(const Circular_arc_point_3 & point,
            typename Critical_event::Tag_type tag) const
        {
          Critical_event ce;
          Event_builder::link_to_sphere(ce);
          link_to_circle(ce);
          ce.point = point;
          ce.tag = tag;
          return ce;
        }

        Polar_event polar_event(const Circular_arc_point_3 & point,
            typename Polar_event::Pole_type pole,
            typename Polar_event::Tag_type tag) const
        {
          Polar_event pe;
          Event_builder::link_to_sphere(pe);
          link_to_circle(pe);
          pe.point = point;
          pe.pole = pole;
          pe.tag = tag;
          return pe;
        }

        Bipolar_event bipolar_event(const Vector_3 & normal,
            typename Bipolar_event::Tag_type tag) const
        {
          Bipolar_event be;
          Event_builder::link_to_sphere(be);
          link_to_circle(be);
          be.normal = normal;
          be.tag = tag;
          return be;
        }

      protected:
        void link_to_circle(Circle_event & ev) const
        { ev.circle = _circle; }

      private:
        Circle_handle _circle;
    };

    // An event site is the location where several events come in
    // conflict, and allows ordering independently of the type of
    // location, but more on the type of the underlying event.

    class Polar_event_site;
    class Bipolar_event_site;

    class Normal_event_site
    {
      // Iterator ranges should have access to hidden members
      friend class Intersection_events_range;

      public:
        // Start/End events
        typedef std::multiset<Critical_event, Comp_event_circle_radii> Start_events;
        typedef std::multiset<Critical_event, Comp_event_inv_circle_radii> End_events;

        // Crossing/Tangency events
        typedef std::vector<Intersection_event> Intersection_events;

        Normal_event_site(const Sphere_handle & s,
            const Circular_arc_point_3 & p):
          _point(p), _sphere(s),
          _start_events(), _end_events(),
          _intersection_events() {}

        // Add a base normal event
        void add_event(const Critical_event &);

        // Overload for adding an intersection event
        void add_event(const Intersection_event &);

        // Done using lexicographic comparing. This introduces
        // the concepts that points are compared lexicographically and are
        // placed in a frame local to the sphere (ie with its origin at the
        // center of the sphere), in cylindrical coordinates.
        bool occurs_before(const Normal_event_site &) const;

        // Symmetric version of Polar_event_site::occurs_before
        bool occurs_before(const Polar_event_site & es) const
        { return es.occurs_before(*this) == false; }
        // same for Bipolar_event_site
        bool occurs_before(const Bipolar_event_site & es) const
        { return es.occurs_before(*this) == false; }

        // Start events iterator range
        typedef typename Start_events::const_iterator
          Start_events_iterator;
        class Start_events_range
        {
          public:
            Start_events_range(const Normal_event_site & nes):
              _nes(nes) {}

            Start_events_iterator begin() const
            { return _nes._start_events.begin(); }

            Start_events_iterator end() const
            { return _nes._start_events.end(); }

          private:
            const Normal_event_site & _nes;
        };

        // End events iterator range
        typedef typename End_events::const_iterator
          End_events_iterator;
        class End_events_range
        {
          public:
            End_events_range(const Normal_event_site & nes):
              _nes(nes) {}

            End_events_iterator begin() const
            { return _nes._end_events.begin(); }

            End_events_iterator end() const
            { return _nes._end_events.end(); }

          private:
            const Normal_event_site & _nes;
        };

        // Intersection events iterator range
        typedef typename Intersection_events::const_iterator
          Intersection_events_iterator;
        class Intersection_events_range
        {
          public:
            Intersection_events_range(const Normal_event_site & nes):
              _nes(nes) {}

            Intersection_events_iterator begin() const
            { return _nes._intersection_events.begin(); }

            Intersection_events_iterator end() const
            { return _nes._intersection_events.end(); }

          private:
            const Normal_event_site & _nes;
        };

        // Access point
        const Circular_arc_point_3 & point() const
        { return _point; }
        // ...sphere
        const Sphere_handle & sphere() const
        { return _sphere; }
        // ...start events
        const Start_events & start_events() const
        { return _start_events; }
        // ...end events
        const End_events & end_events() const
        { return _end_events; }
        // ...intersection events
        const Intersection_events & intersection_events() const
        { return _intersection_events; }

      private:
        // Location of event site
        Circular_arc_point_3 _point;
        // ...source sphere
        Sphere_handle _sphere;

        // Start/End events
        Start_events _start_events;
        End_events _end_events;

        // Crossing/Tangency events
        Intersection_events _intersection_events;
    };

    class Polar_event_site
    {
      public:
        Polar_event_site() {}

        Polar_event_site(const Polar_event & ev):
          _event(ev) {}

        bool occurs_before(const Normal_event_site &) const
        { return _event.is_end(); }

        bool occurs_before(const Polar_event_site &) const;

        bool occurs_before(const Bipolar_event_site & es) const
        { return es.occurs_before(*this) == false; }

        const Polar_event & event() const
        { return _event; }

      private:
        Polar_event _event;
    };

    class Bipolar_event_site
    {
      public:
        Bipolar_event_site() {}

        Bipolar_event_site(const Bipolar_event & ev):
          _event(ev) {}

        bool occurs_before(const Normal_event_site &) const
        { return true; }

        bool occurs_before(const Polar_event_site & es) const
        { return es.event().is_end() == false; }

        bool occurs_before(const Bipolar_event_site &) const;

        const Bipolar_event & event() const
        { return _event; }

      private:
        Bipolar_event _event;
    };
};

template <typename SK>
class Event_queue
{
  typedef typename SK::Circular_arc_point_3 Circular_arc_point_3;

  public:
    typedef Event_bundle<SK> Events;
    typedef typename Events::Normal_event_site Normal_event_site;
    typedef typename Events::Polar_event_site Polar_event_site;
    typedef typename Events::Bipolar_event_site Bipolar_event_site;

    // Helper enum for returning either one of the type
    // of events that can be handled by the queue, or none
    enum Event_site_type {
      None, Normal, Polar, Bipolar
    };

    // Wrapper for any event site in the queue
    class Any_event_site
    {
      public:
        Any_event_site():
          _impl(0, None) {}
        Any_event_site(const Normal_event_site & nes):
          _impl(new Normal_event_site(nes), Normal) {}
        Any_event_site(const Polar_event_site & pes):
          _impl(new Polar_event_site(pes), Polar) {}
        Any_event_site(const Bipolar_event_site & bpes):
          _impl(new Bipolar_event_site(bpes), Bipolar) {}

        Any_event_site(const Any_event_site & aes);

        ~Any_event_site();
        Any_event_site & operator=(const Any_event_site &);

        // Get the underlying type
        Event_site_type type() const
        { CGAL_assertion(_impl.second != None);
          return _impl.second; }

        // Conversion methods, check assertions in debug mode
        const Normal_event_site & as_normal() const
        { CGAL_assertion(_impl.second == Normal);
          return *static_cast<const Normal_event_site *>(_impl.first); }
        const Polar_event_site & as_polar() const
        { CGAL_assertion(_impl.second == Polar);
          return *static_cast<const Polar_event_site *>(_impl.first); }
        const Bipolar_event_site & as_bipolar() const
        { CGAL_assertion(_impl.second == Bipolar);
          return *static_cast<const Bipolar_event_site *>(_impl.first); }

        bool operator<(const Any_event_site & e) const;
        bool operator<(const Normal_event_site & nes) const;
        bool operator<(const Polar_event_site & pes) const;
        bool operator<(const Bipolar_event_site & bpes) const;

      private:
        std::pair<void *, Event_site_type> _impl;
    };

  private:
    // Actual queue implementation
    typedef std::priority_queue<Any_event_site> Event_site_queue;

  public:
    // STL container concept requirements (delegation)
    typedef typename Event_site_queue::value_type value_type;
    typedef typename Event_site_queue::reference reference;
    typedef typename Event_site_queue::const_reference const_reference;
    typedef typename Event_site_queue::size_type size_type;

    bool empty() const
    { return _queue.empty(); }

    size_type size() const
    { return _queue.size(); }

    void clear()
    { _queue = Event_site_queue(); }

    // Push normal events to the queue
    void push(const Normal_event_site & nes)
    { _queue.push(nes); }
    // ...push polar events
    void push(const Polar_event_site & pes)
    { _queue.push(pes); }
    // ...push bipolar events
    void push(const Bipolar_event_site & bpes)
    { _queue.push(bpes); }

    // Type of the next event in the queue
    Event_site_type next_event() const
    { return empty() == false ? _queue.top().type() : None; }

    // Top/Pop normal
    const Normal_event_site & top_normal() const;
    Normal_event_site pop_normal();

    // Top/Pop polar
    const Polar_event_site & top_polar() const;
    Polar_event_site pop_polar();

    // Top/Pop bipolar
    const Bipolar_event_site & top_bipolar() const;
    Bipolar_event_site pop_bipolar();

  private:
    Event_site_queue _queue;
};

#endif // EVENT_QUEUE_H // vim: sw=2 et ts=2 sts=2
