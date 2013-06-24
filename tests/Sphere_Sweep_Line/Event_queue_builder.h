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
  typedef typename Kernel::Point_3 Point_3;
  typedef typename Kernel::Circle_3 Circle_3;
  typedef typename Kernel::Sphere_3 Sphere_3;
  typedef typename Kernel::Direction_3 Direction_3;
  typedef typename Kernel::Circular_arc_3 Circular_arc_3;
  typedef typename Kernel::Circular_arc_point_3 Circular_arc_point_3;

  typedef typename Kernel::Object_3 Object_3;
  typedef typename Kernel::Assign_3 Assign_3;
  typedef typename Kernel::Intersect_3 Intersect_3;
  //typedef typename Kernel::CompareThetaZ_3 CompareThetaZ_3;

  typedef Sphere_intersecter<Kernel> SI;
  typedef typename SI::Sphere_handle Sphere_handle;
  typedef typename SI::Circle_handle Circle_handle;
  typedef Event_queue_builder<Kernel> Self;
  typedef Event_queue<Kernel> EQ;

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

      // Check basic assertions
      CGAL_assertion(_si != 0);
      CGAL_assertion(_si->find_sphere(*sh).is_null() == false);

      // Normal event sites, ordered by corresponding point
      typedef Normal_event_site<Kernel> NES;
      typedef std::map<Circular_arc_point_3, NES> NES_map;
      NES_map nes;

#define ADD_IE_TO_SITE(POINT, TYPE)                              \
      { typedef std::pair<const Circular_arc_point_3, NES> pair; \
        typename NES_map::iterator it = nes.find(POINT);         \
        if (it == nes.end())                                     \
        { it = nes.insert(std::make_pair(POINT,                  \
              NES(sh, POINT))).first; }                          \
        it->second.add_event(IE(c1, c2, IE::TYPE)); }

      // Make start/end events
      // TODO

      // Make intersection events
      typedef std::vector<Circle_handle> Circle_list;

      // Get the sphere's circles
      Circle_list circle_list;
      _si->circles_on_sphere(sh, circle_list.begin());

      // Do all intersections
      for (typename Circle_list::const_iterator it = circle_list.begin();
          it != circle_list.end(); it++)
      {
        const Circle_handle & ch1 = *it;
        for (typename Circle_list::const_iterator it2 = it + 1;
            it2 != circle_list.end(); it2++)
        {
          // *More* syntaxic sugar
          typedef Intersection_event<Kernel> IE;
          const Circle_handle & ch2 = *it2;
          const Circle_3 & c1 = *ch1;
          const Circle_3 & c2 = *ch2;

          // Intersection circles must be different
          CGAL_assertion(ch1 != ch2 && c1 != c2);

          // Do intersections
          std::vector<Object_3> intersected;
          Intersect_3()(c1, c2, std::back_inserter(intersected));

          // Handle intersections
          if (intersected.empty())
          { continue; }
          else if (intersected.size() == 1) // Equality or Tangency
          {
            // Test if intersection is a point -> tangency
            std::pair<Circular_arc_point_3, unsigned int> cap;
            if (Assign_3()(cap, intersected[0]))
            {
              // Handle circle tangency
              ADD_IE_TO_SITE(cap.first, Tangency);
              continue;
            }

            // Intersection is necessarily a circle
            Circle_3 c;
            CGAL_assertion(Assign_3()(c, intersected[0]));
            //handle_circle_equality(ev_queue, c1, c2); FIXME
          }
          else // Crossing
          {
            // There is necessarily two intersections
            CGAL_assertion(intersected.size() == 2);

            std::pair<Circular_arc_point_3, unsigned int> cap1, cap2;
            CGAL_assertion(Assign_3()(cap1, intersected[0])
                && Assign_3()(cap2, intersected[1]));
            CGAL_assertion(cap1.second == 1 && cap2.second == 1);

            // Handle circle crossing
            // ...first point
            ADD_IE_TO_SITE(cap1.first, Largest_crossing);
            ADD_IE_TO_SITE(cap1.first, Smallest_crossing);
            // ...second point
            ADD_IE_TO_SITE(cap2.first, Largest_crossing);
            ADD_IE_TO_SITE(cap2.first, Smallest_crossing);
          }
        }
      }
#undef ADD_IE_TO_SITE
      return ev_queue;
    }

  private:
    const SI * _si;
    Direction_3 _axis_dir;
};

#endif // EVENT_QUEUE_BUILDER_H // vim: sw=2 et ts=2 sts=2
