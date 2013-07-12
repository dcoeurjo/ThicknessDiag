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
  typedef Sphere_intersecter<SK> SI;
  typedef typename SI::Circle_handle Circle_handle;
  typedef typename SI::Sphere_handle Sphere_handle;

  // Geometric objects
  typedef typename SK::Circle_3 Circle_3;
  typedef typename SK::Sphere_3 Sphere_3;
  typedef typename SK::Circular_arc_point_3 Circular_arc_point_3;

  public:
    // Base class for all events, holding a link to the source sphere
    // TODO decide if this is really necessary
    struct Sphere_event
    {
      Sphere_event(const Sphere_handle & s):
        sphere(s) {}

      Sphere_handle sphere;

      bool operator==(const Sphere_event & ev) const
      { return sphere == ev.sphere; }
    };

    // Used to refactor comparing between events which
    // are linked to a single circle as source
    struct Circle_event: Sphere_event
    {
      Circle_handle circle;

      bool operator==(const Circle_event & ev) const
      { return circle == ev.circle; }
    };

    // Used to mark events with a { Start, End } tag
    struct Start_end_event
    {
      enum Tag_type {
        Start, End
      };

      Tag_type tag;

      bool is_start() const
      { return tag == Start; }

      bool is_end() const
      { return tag == End; }

      bool operator==(const Start_end_event & ev) const
      { return tag == ev.tag; }
    };

    // Cirtical normal events are defined by:
    //  - the corresponding circle link
    //  - a tag { Start, End }
    struct Critical_event: Circle_event, Start_end_event
    {
      bool operator==(const Critical_event & ev) const
      { return Circle_event::operator==(ev)
        && Start_end_event::operator==(ev); }
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


      Circle_handle_pair circles;
      Intersection_type type;

      bool operator==(const Intersection_event & ev) const
      { return type == ev.type && circles == ev.circles; }
    };

    // Polar events are defined by:
    //  - the corresponding circle link
    //  - a tag { Start, End }
    //  - a pole { North, South }
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
    //  - the corresponding circle link
    //
    // FIXME no extra data ?
    struct Bipolar_event: Circle_event {};

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
        Circle_event_builder prepare_circle_event(const Circle_handle &) const;

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

        Critical_event critical_event(Critical_event::Tag_type tag) const
        {
          Critical_event ce;
          link_to_sphere(ce);
          link_to_circle(ce);
          ce.tag = tag;
          return ce;
        }

        Polar_event polar_event(Polar_event::Pole_type pole, Polar_event::Tag_type tag) const
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
        { ev.circle = circle; }

      private:
        Circle_handle _circle;
    };

    Circle_event_builder Event_builder::prepare_circle_event(const Circle_handle & c) const
    { return Circle_event_builder(c, _sphere); }

    // An event site is the location where several events come in
    // conflict, and allows ordering independently of the type of
    // location, but more on the type of the underlying event.

    class Polar_event_site;

    class Normal_event_site
    {
      // Iterator ranges should have access to hidden members
      friend class Intersection_events_range;

      // Compare theta-z
      typedef Comp_theta_z_3<SK> Compare_theta_z_3;

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
        { return _event.is_bipolar() || _event.is_end(); }

        bool occurs_before(const Polar_event_site & es) const
        {
          if (_event.is_bipolar() != es._event.is_bipolar()) // Different polarities
          {
            if (_event.is_bipolar() == false)
            { return _event.is_end(); } // Polar end before bipolar
            else
            { return es._event.is_start(); } // Bipolar only before polar start
          }
          else if (_event.is_bipolar() == false) // Both "single" polar
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
          else // Both bipolar
          {
            return false; // FIXME two bipolar events cannot be in conflict (?)
          }
        }

      private:
        Polar_event _event;
    };
}

template <typename SK>
class Event_queue
{
  public:
    typedef Events_bundle<SK> Events;
    typedef typename Events::Normal_event_site Normal_event_site;
    typedef typename Events::Polar_event_site Polar_event_site;
    typedef typename Events::Bipolar_event_site Bipolar_event_site;

    // Helper enum for returning either one of the type
    // of events that can be handled by the queue, or none
    enum Event_site_type {
      None, Normal, Polar, Bipolar
    };

    Event_queue():
      _normal_events(), _polar_events(),
      _computed_state(None) {}

    // Push events to our queue
    void push(const Normal_event_site & es)
    { _normal_events.push(es);
      update_state(); }
    void push(const Polar_event_site & es)
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
    Polar_event_site pop_polar()
    {
      Polar_event_site pes = _polar_events.top();
      _polar_events.pop();
      update_state();
      return pes;
    }

    // Get a polar event from the queue (without removing it)
    // Concept: there is at least one polar event
    const Polar_event_site & top_polar() const
    { return _polar_events.top(); }

    // Pop a normal event from the queue
    // Concept: there is at least one normal event
    Normal_event_site pop_normal()
    {
      Normal_event_site nes = _normal_events.top();
      _normal_events.pop();
      update_state();
      return nes;
    }

    // Get a normal event from the queue (without removing it)
    // Concept: there is at least one normal event
    const Normal_event_site & top_normal() const
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
      const Normal_event_site & nes = _normal_events.top();
      const Polar_event_site & pes = _polar_events.top();
      if (nes.occurs_before(pes))
      { return Normal; }
      else
      { return Polar; }
    }

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
        typedef std::priority_queue<Event_site, std::vector<Event_site>,
                Comp_event_sites<Event_site> > Type;
    };

    // Normal event sites
    typename Event_site_queue<Normal_event_site>::Type _normal_events;

    // Polar event sites
    typename Event_site_queue<Polar_event_site>::Type _polar_events;

    // Event queue state, reset after each pop/push, computed
    Event_site_type _computed_state;
};

#endif // EVENT_QUEUE_H // vim: sw=2 et ts=2 sts=2
