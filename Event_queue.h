#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <set>
#include <queue>
#include <vector>
#include <utility>
#include <functional>

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
  typedef typename SK::Sphere_3 Sphere_3;
  typedef typename SK::Circular_arc_point_3 Circular_arc_point_3;


  // Function objects
  typedef Comp_theta_z_3<SK> Compare_theta_z_3;

  public:
    // Base class for all events, holding a link to the source sphere
    struct Sphere_event
    {
      Sphere_event(const Sphere_handle & s):
        sphere(s) {}

      Sphere_handle sphere;

      bool operator==(const Sphere_event & ev) const
      { return sphere == ev.sphere; }
    };

    // CRTP for events which occur at a particular point on a sphere
    //
    // Concepts :
    //  - Derived defines a point() method, returning the point where the
    //    event is located (which must be on the sphere of course)
    //  - Derived has a sphere data member (which is a Sphere_handle), for
    //    example by deriving from Sphere_event as a mixin
    template <typename Derived>
    struct Point_event
    {
      const Circular_arc_point_3 & point() const
      { return static_cast<const Derived &>(*this).point(); }

      const Sphere_handle & sphere() const
      { return static_cast<const Derived &>(*this).sphere; }

      template <typename Other_derived>
      bool operator<(const Point_event<Other_derived> & ev) const
      { CGAL_assertion(sphere() == ev.sphere());
        return Compare_theta_z_3()(point(), ev.point(), *sphere()); }
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
      // TODO
    };

    // Intersection normal events are defined by:
    //  - a tag { Smallest_crossing, Largest_crossing, Tangency }
    //  - the pair of circles intersecting
    struct Intersection_event: Point_event<Intersection_event>, Sphere_event
    {
      typedef std::pair<Circle_handle, Circle_handle> Circle_handle_pair;

      enum Intersection_type {
        Smallest_crossing,
        Largest_crossing,
        Tangency
      };

      Intersection_type type;
      Circle_handle_pair circles;
      Circular_arc_point_3 intersection_point;

      const Circular_arc_point_3 & point() const
      { return intersection_point; }

      bool operator==(const Intersection_event & ev) const
      { return type == ev.type && circles == ev.circles;
        && intersection_point == ev.intersection_point; }
    };

    // Polar events are defined by:
    //  - a pole { North, South }
    //  - a critical event's data
    struct Polar_event: Point_event<Polar_event>, Circle_event
    {
      enum Pole_type {
        North, South
      };

      Pole_type pole;
      Circular_arc_point_3 pole_point;

      const Circular_arc_point_3 & point() const
      { return pole_point; }

      bool is_north() const
      { return pole == North; }

      bool is_south() const
      { return pole == South; }

      bool operator==(const Polar_event & ev) const
      { return Circle_event::operator==(ev)
        && pole == ev.pole && pole_point == ev.pole_point; }
    };

    // Bipolar events are defined by:
    struct Bipolar_event: Circle_event
    {
      // TODO add angle/circular arc member
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
        Intersection_event intersection_event(const Circle_handle & first,
            const Circle_handle & second) const
        {
          Intersection_event ie;
          link_to_sphere(ie);
          ie.circles = Intersection_event::Circle_handle_pair(first, second);
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

        Critical_event critical_event(typename Critical_event::Tag_type tag) const
        {
          Critical_event ce;
          link_to_sphere(ce);
          link_to_circle(ce);
          ce.tag = tag;
          return ce;
        }

        Polar_event polar_event(typename Polar_event::Pole_type pole,
            typename Polar_event::Tag_type tag) const
        {
          Polar_event pe;
          link_to_sphere(pe);
          link_to_circle(pe);
          pe.pole = pole;
          pe.tag = tag;
          return pe;
        }

        Bipolar_event bipolar_event() const
        {
          Bipolar_event be;
          link_to_sphere(be);
          link_to_circle(be);
          return be;
        }

      protected:
        void link_to_circle(const Circle_event & ev) const
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

      // Start/End events
      typedef std::multiset<Critical_event, Comp_event_circle_radii> Start_events;
      typedef std::multiset<Critical_event, Comp_event_inv_circle_radii> End_events;

      // Crossing/Tangency events
      typedef std::vector<Intersection_event> Intersection_events;

      public:
        Normal_event_site(const Sphere_handle & s,
            const Circular_arc_point_3 & p):
          _point(p), _sphere(s),
          _start_events(), _end_events(),
          _intersection_events() {}

        // Add a base normal event
        void add_event(const Critical_event & ev)
        {
          CGAL_assertion(_sphere == ev.sphere);
          if (ev.is_start()) { _start_events.insert(ev); }
          else { _end_events.insert(ev); }
        }

        // Overload for adding an intersection event
        void add_event(const Intersection_event & ev)
        {
          CGAL_assertion(_sphere == ev.sphere);
          _intersection_events.push_back(ev);
        }

        // Range version of add_event taking any iterator
        // verifying the STL InputIterator concept
        template <typename InputIterator>
        void add_event(InputIterator begin, InputIterator end)
        {
          for (; begin != end; begin++)
          { add_event(*begin); }
        }

        // Done using lexicographic comparing. This introduces
        // the concepts that points are compared lexicographically and are
        // placed in a frame local to the sphere (ie with its origin at the
        // center of the sphere), in cylindrical coordinates.
        bool occurs_before(const Normal_event_site & es) const
        { return Compare_theta_z_3()(_point, es._point, *_sphere); }

        // Symmetric version of Polar_event_site::occurs_before
        bool occurs_before(const Polar_event_site & es) const
        { return es.occurs_before(*this) == false; }
        // same for Bipolar_event_site
        bool occurs_before(const Bipolar_event_site & es) const
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
            Intersection_events_range(const Normal_event_site & nes):
              _nes(nes) {}

            Intersection_events_iterator begin() const
            { return _nes._intersection_events.begin(); }

            Intersection_events_iterator end() const
            { return _nes._intersection_events.end(); }

          private:
            const Normal_event_site & _nes;
        };

        // Get the corresponding sphere handle
        const Sphere_handle & sphere() const
        { return _sphere; }

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
        Polar_event_site(const Polar_event & ev):
          _event(ev) {}

        bool occurs_before(const Normal_event_site &) const
        { return _event.is_end(); }

        bool occurs_before(const Polar_event_site & es) const
        {
          if (_event.tag != es._event.tag)
          { return _event.is_end(); } // End before start
          else if (_event.pole != es._event.pole)
          { return _event.is_north(); } // North pole before south
          else
          {
            // Store the polar event site having the biggest
            // associated circle radius
            const Polar_event_site * es_biggest_circle = this;
            typedef Comp_by_squared_radii<Circle_3> Comp_circle_radii;
            if (Comp_circle_radii()(*es._event.circle, *_event.circle))
            { es_biggest_circle = &es; }

            // Start -> biggest occurs first, end -> smallest occurs first
            if (_event.is_start())
            { return es_biggest_circle == this; }
            else
            { return !(es_biggest_circle == this); }
          }
        }

        bool occurs_before(const Bipolar_event_site & es) const
        { return es.occurs_before(*this) == false; }

      private:
        Polar_event _event;
    };

    class Bipolar_event_site
    {
      public:
        Bipolar_event_site(const Bipolar_event & ev):
          _event(ev) {}

        bool occurs_before(const Normal_event_site &) const
        { return true; }

        bool occurs_before(const Polar_event_site & es) const
        { return es.is_end() == false; }

        bool occurs_before(const Bipolar_event_site &) const
        {
          // TODO
        }

      private:
        Bipolar_event _event;
    };
};

// TODO this is completely broken, event sites should be ordered globally,
// before ordering by type for events in conflict
template <typename SK>
class Event_queue
{
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

    // Push normal events to the queue
    void push(const Normal_event_site & es)
    { _normal_event_sites.push(es); }
    // ...push bipolar events
    void push(const Bipolar_event_site & es)
    { _bipolar_event_sites.push(es); }
    // ...push polar events
    void push(const Polar_event_site & es)
    {
      if (es.is_start())
      { _polar_event_sites_pair.first.push(es); }
      else
      {
        CGAL_assertion(es.is_end());
        _polar_event_sites_pair.second.push(es);
      }
    }

    // Helper for checking if event queue is empty
    bool empty() const
    { return next_event() == None; }

    // Get the type of the next event to handle,
    // order of event handling is as follows :
    //  - polar "end" event sites
    //  - bipolar event sites
    //  - normal event sites
    //  - polar "start" event sites
    Event_site_type next_event() const
    {
      if (_polar_event_sites_pair.second.empty() == false)
      { return Polar; }
      else if (_bipolar_event_sites.empty() == false)
      { return Bipolar; }
      else if (_normal_event_sites.empty() == false)
      { return Normal; }
      else if (_polar_event_sites_pair.second.empty() == false)
      { return Polar; }
      else
      { return None; }
    }

    // Pop a polar event from the queue
    // Concept: there is at least one polar event
    Polar_event_site pop_polar()
    {
      Polar_event_site pes;
      if (_polar_event_sites_pair.first.empty() == false)
      {
        pes = _polar_event_sites_pair.first.top();
        _polar_event_sites_pair.first.pop();
      }
      else
      {
        CGAL_assertion(_polar_event_sites_pair.second.empty() == false);
        pes = _polar_event_sites_pair.second.top();
        _polar_event_sites_pair.second.pop();
      }
      return pes;
    }

    // Get a polar event from the queue (without removing it)
    // Concept: there is at least one polar event
    const Polar_event_site & top_polar() const
    {
      if (_polar_event_sites_pair.first.empty() == false)
      { return _polar_event_sites_pair.first.top(); }
      else
      { CGAL_assertion(_polar_event_sites_pair.second.empty() == false);
        return _polar_event_sites_pair.second.top(); }
    }

    // Pop a normal event from the queue
    // Concept: there is at least one normal event
    Normal_event_site pop_normal()
    {
      Normal_event_site nes = _normal_event_sites.top();
      _normal_event_sites.pop();
      return nes;
    }

    // Get a normal event from the queue (without removing it)
    // Concept: there is at least one normal event
    const Normal_event_site & top_normal() const
    { return _normal_event_sites.top(); }

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
    class Event_site_queue
    {
      struct Comp_event_sites: std::unary_function<bool, Event_site>
      {
        bool operator()(const Event_site & left,
            const Event_site & right) const
        { return right.occurs_before(left); }
      };

      public:
        typedef std::priority_queue<Event_site,
                std::vector<Event_site>, Comp_event_sites> Type;
    };

    // Normal event sites
    typename Event_site_queue<Normal_event_site>::Type _normal_event_sites;

    // Bipolar event sites
    typename Event_site_queue<Bipolar_event_site>::Type _bipolar_event_sites;

    // Polar event sites (first -> start, second -> end)
    typedef typename Event_site_queue<Polar_event_site>::Type
      Polar_event_sites_queue;
    std::pair<Polar_event_sites_queue, Polar_event_sites_queue>
      _polar_event_sites_pair;
};

#endif // EVENT_QUEUE_H // vim: sw=2 et ts=2 sts=2
