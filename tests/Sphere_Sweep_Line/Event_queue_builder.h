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
  typedef typename Kernel::CompareThetaZ_3 CompareThetaZ_3;

  typedef Event_queue_builder<Kernel> Self;
  typedef Event_queue<Kernel> EQ;
  typedef Sphere_intersecter<Kernel> SI;

  typedef typename SI::Sphere_handle Sphere_handle;
  typedef typename SI::Circle_handle Circle_handle;

  void make_start_end_events(const Sphere_handle & sh)
  {
    // TODO
  }

  void make_crossing_tangency_events(EQ & ev_queue,
      const Sphere_handle & sh)
  {
    // Cleaner code with typedef
    typedef std::vector<Circle_handle> Circle_list;

    // Get the sphere's circles
    Circle_list circle_list;
    _si->circles_on_sphere(sh, circle_list.begin());

    // Do all intersections
    for (typename Circle_list::const_iterator it = circle_list.begin();
        it != circle_list.end(); it++)
    { for (typename Circle_list::const_iterator it2 = it + 1;
          it2 != circle_list.end(); it2++)
      { do_circle_intersection(ev_queue, *it, *it2); } }
  }

  void do_circle_intersection(EQ & ev_queue,
      const Circle_handle & ch1,
      const Circle_handle & ch2)
  {
    // *More* syntaxic sugar
    const Circle_3 & c1 = *ch1;
    const Circle_3 & c2 = *ch2;

    // Intersection circles must be different
    CGAL_assertion(ch1 != ch2 && c1 != c2);

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
      { handle_circle_tangency(ev_queue, c1, c2, cap.first);
        return; }

      // Intersection is necessarily a circle
      Circle_3 c;
      CGAL_assertion(Assign_3()(c, intersected[0]));
      handle_circle_equality(ev_queue, c1, c2);
    }
    else // Crossing
    {
      // There is necessarily two intersections
      CGAL_assertion(intersected.size() == 2);

      std::pair<Circular_arc_point_3, unsigned int> cap1, cap2;
      CGAL_assertion(Assign_3()(cap1, intersected[0])
          && Assign_3()(cap2, intersected[1]));
      CGAL_assertion(cap1.second == 1 && cap2.second == 1);
      handle_circle_crossing(ev_queue, c1, c2,
          std::make_pair(cap1.first, cap2.first));
    }
  }

  void handle_circle_equality(EQ & ev_queue,
      const Circle_3 & c1, const Circle_3 & c2) const
  {
    // TODO
  }

  void handle_circle_crossing(EQ & ev_queue, 
      const Circle_3 & c1, const Circle_3 & c2,
      const std::pair<Circular_arc_point_3,
      Circular_arc_point_3> & points) const
  {
    // Largest (smallest) events for the two points
    typedef Intersection_event<Kernel> IE;
    // ...first point
    ev_queue.push(IE(points.first, c1, c2, IE::Largest_crossing));
    ev_queue.push(IE(points.first, c1, c2, IE::Smallest_crossing));
    // ...second point
    ev_queue.push(IE(points.second, c1, c2, IE::Largest_crossing));
    ev_queue.push(IE(points.second, c1, c2, IE::Smallest_crossing));
  }

  void handle_circle_tangency(EQ & ev_queue,
      const Circle_3 & c1, const Circle_3 & c2,
      const Circular_arc_point_3 & cap) const
  {
    typedef Intersection_event<Kernel> IE;
    ev_queue.push(IE(cap, c1, c2, IE::Tangency));
  }

  public:
    Event_queue_builder(const SI * si = 0):
      _si(si), _axis_dir(0, 0, 1) {}

    Event_queue_builder(const Self &);
    Self & operator=(const Self &);

    Self & with_si(const SI & si)
    { _si = &si;
      return *this; }

    Self & with_pole_axis(const Direction_3 & d)
    { _axis_dir = d; 
      return *this; }

    EQ build(const Sphere_handle & sh)
    {
      EQ ev_queue;
      CGAL_assertion(_si != 0);
      CGAL_assertion(_si->find_sphere(*sh).is_null() == false);
      make_start_end_events(sh, ev_queue);
      make_crossing_tangency_events(sh, ev_queue);
      return ev_queue;
    }

  private:
    const SI * _si;
    Direction_3 _axis_dir;
};

#endif // EVENT_QUEUE_BUILDER_H // vim: sw=2 et ts=2 sts=2
