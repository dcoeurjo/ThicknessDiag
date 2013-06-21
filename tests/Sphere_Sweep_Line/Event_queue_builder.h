#ifndef EVENT_QUEUE_BUILDER_H
#define EVENT_QUEUE_BUILDER_H

#include <vector>
#include <utility>
#include <iterator>

#include <CGAL/assertions.h>

#include <Event_queue.h>
#include <Sphere_intersecter.h>

template <typename Kernel>
class Event_queue_builder
{
  typedef typename Kernel::Circle_3 Circle_3;
  typedef typename Kernel::Sphere_3 Sphere_3;
  typedef typename Kernel::Direction_3 Direction_3;
  typedef typename Kernel::Circular_arc_3 Circular_arc_3;
  typedef typename Kernel::Circular_arc_point_3 Circular_arc_point_3;

  typedef typename Kernel::Object_3 Object_3;
  typedef typename Kernel::Assign_3 Assign_3;
  typedef typename Kernel::Intersect_3 Intersect_3;

  typedef Sphere_intersecter<Kernel> SI;
  typedef typename SI::Sphere_handle Sphere_handle;

  void make_start_end_events(const Sphere_handle & sh)
  {
  }

  void make_crossing_tangency_events(const Sphere_handle & sh)
  {
  }

  void do_circle_intersection(const Circle_3 & c1, const Circle_3 & c2)
  {
    // Intersection circles must be different
    CGAL_assertion(c1 != c2);

    // Do intersections
    std::vector<Object_3> intersected;
    Intersect_3()(c1, c2, std::back_inserter(intersected));

    // Handle intersections
    if (intersected.empty())
    { return; }
    else if (intersected.size() == 1) // Equality or Tangency
    {
      // Test if intersection is a point -> tangency
      std::pair<Circular_arc_point_3, unsigned int> cap;
      if (Assign_3()(cap, intersected[0]))
      { handle_circle_tangency(c1, c2, cap.first);
        return; }

      // Intersection is necessarily a circle
      Circle_3 c;
      CGAL_assertion(Assign_3()(c, intersected[0]));
      handle_circle_equality(c1, c2);
    }
    else // Crossing
    {
      // There is necessarily two intersections
      CGAL_assertion(intersected.size() == 2);

      std::pair<Circular_arc_point_3, unsigned int> cap1, cap2;
      CGAL_assertion(Assign_3()(cap1, intersected[0])
          && Assign_3()(cap2, intersected[1]));
      CGAL_assertion(cap1.second == 1 && cap2.second == 1);
      handle_circle_crossing(c1, c2, std::make_pair(cap1.first, cap2.first));
    }
  }

  void handle_circle_tangency(const Circle_3 & c1, const Circle_3 & c2,
      const Circular_arc_point_3 & cap)
  {
  }

  void handle_circle_equality(const Circle_3 & c1, const Circle_3 & c2)
  {
  }

  void handle_circle_crossing(const Circle_3 & c1, const Circle_3 & c2,
      const std::pair<Circular_arc_point_3, Circular_arc_point_3> &)
  {
  }

  public:
    Event_queue_builder(const SI & si):
      _si(si), _axis_dir(0, 0, 1) {}

    Event_queue_builder & with_pole_axis(const Direction_3 & d)
    { _axis_dir = d; }

    Event_queue<Kernel> build(const Sphere_handle & sh)
    {
      Event_queue<Kernel> ev_queue;
      CGAL_assertion(_si.find_sphere(*sh).is_null() == false);
      make_start_end_events(sh);
      make_crossing_tangency_events(sh);
      return ev_queue;
    }

  private:
    SI & _si;
    Direction_3 _axis_dir;
};

#endif // EVENT_QUEUE_BUILDER_H // vim: sw=2 et ts=2 sts=2
