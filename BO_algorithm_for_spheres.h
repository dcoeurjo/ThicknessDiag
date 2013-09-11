#ifndef BO_ALGORITHM_FOR_SPHERES_H
#define BO_ALGORITHM_FOR_SPHERES_H

#include <vector>
#include <algorithm>

#include <boost/thread.hpp>

#include <Sphere_intersecter.h>
#include <Event_queue.h>

template <typename SK>
class BO_algorithm_for_spheres
{
  typedef BO_algorithm_for_spheres Self;

  // Geometrical objects
  typedef typename SK::Assign_3 Assign_3;
  typedef typename SK::Circle_3 Circle_3;
  typedef typename SK::Circular_arc_3 Circular_arc_3;
  typedef typename SK::Circular_arc_point_3 Circular_arc_point_3;
  typedef typename SK::Compare_theta_3 Compare_theta_3;
  typedef typename SK::Direction_3 Direction_3;
  typedef typename SK::Intersect_3 Intersect_3;
  typedef typename SK::Line_3 Line_3;
  typedef typename SK::Object_3 Object_3;
  typedef typename SK::Plane_3 Plane_3;
  typedef typename SK::Point_3 Point_3;
  typedef typename SK::Sphere_3 Sphere_3;
  typedef typename SK::Vector_3 Vector_3;

  // Sphere intersecter
  typedef Sphere_intersecter<SK> SI;
  typedef typename SI::Circle_handle Circle_handle;
  typedef typename SI::Sphere_handle Sphere_handle;

  // Event queue
  typedef Event_queue<SK> EQ;
  typedef typename EQ::Events Events;
  typedef typename EQ::Event_site_type Event_site_type;
  typedef typename Events::Bipolar_event Bipolar_event;
  typedef typename Events::Bipolar_event_site Bipolar_event_site;
  typedef typename Events::Circle_event_builder Circle_event_builder;
  typedef typename Events::Critical_event Critical_event;
  typedef typename Events::Event_builder Event_builder;
  typedef typename Events::Intersection_event Intersection_event;
  typedef typename Events::Normal_event_site Normal_event_site;
  typedef typename Events::Polar_event Polar_event;
  typedef typename Events::Polar_event_site Polar_event_site;

  // V-ordering
  typedef std::list<Circular_arc_3> Vorder;

  // Other helpers
  typedef std::vector<Object_3> Intersection_list;
  typedef std::vector<Circle_handle> Circle_handle_list;

  // Handle a normal event site
  void handle_event_site(const Normal_event_site &);
  // ... same, but with a polar/bipolar event site
  void handle_polar_event_site(const Polar_event_site &);
  void handle_bipolar_event_site(const Bipolar_event_site &);

  // Break adjacencies for an event site
  void break_adjacencies(const Normal_event_site &);
  void break_adjacencies(const Polar_event_site &);

  // Initialize event queue
  void initialize_E(const Sphere_handle &, const Circle_handle_list &);

  // Initialize V-ordering
  void initialize_V(const Sphere_handle &, const Circle_handle_list &);

  // Structure used for ordering intersected arcs by a certain point
  // in the initalization of the V structure
  struct Intersected_arc
  {
    typedef typename SK::Compare_theta_z_3 Compare_theta_z_3;

    Intersected_arc(const Sphere_3 & s,
        const Circular_arc_point_3 & p,
        const Circular_arc_3 & arc):
      sphere(s), point(p), arc(arc) {}

    bool operator<(const Intersected_arc & c) const
    {
      CGAL_assertion(&sphere == &c.sphere && sphere == c.sphere);
      return Compare_theta_z_3(sphere)(point, c.point) == CGAL::SMALLER;
    }

    const Sphere_3 & sphere;
    Circular_arc_point_3 point;
    Circular_arc_3 arc;
  };

  // Sphere intersecter
  SI _SI;
  Vorder _V;
  EQ _E;
  Circular_arc_3 _M0;

  public:
    BO_algorithm_for_spheres():
      _SI(), _V(), _E(), _M0() {}
    template <typename InputIterator>
    BO_algorithm_for_spheres(InputIterator begin, InputIterator end):
      _SI(begin, end), _V(), _E(), _M0() {}

    // Add a single sphere
    Sphere_handle add_sphere(const Sphere_3 & sphere)
    { return _SI.add_sphere(sphere); }
    // ...or a range of sphere
    template <typename InputIterator>
    void add_sphere(InputIterator begin, InputIterator end)
    { std::copy(begin, end, _SI.insert_iterator()); }

    void run_for(const Sphere_3 &);
    void run_for(const Sphere_handle &);
};

#endif // BO_ALGORITHM_FOR_SPHERES_H // vim: ft=cpp et sw=2 sts=2
