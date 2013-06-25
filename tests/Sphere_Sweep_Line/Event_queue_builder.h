#ifndef EVENT_QUEUE_BUILDER_H
#define EVENT_QUEUE_BUILDER_H

#include <vector>
#include <utility>
#include <iterator>

#include <CGAL/assertions.h>
#include <CGAL/Kernel/global_functions_3.h>

#ifndef NDEBUG
# define POSSIBLY_ASSERT(S) S
#else
#  define POSSIBLY_ASSERT(S) CGAL_assertion(S);
#endif // NDEBUG

#include <Event_queue.h>
#include <Sphere_intersecter.h>

template <typename Kernel>
class Event_queue_builder
{
  typedef typename Kernel::Point_3 Point_3;
  typedef typename Kernel::Line_3 Line_3;
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
      CGAL_assertion(sh.is_null() == false);
      CGAL_assertion(_si->find_sphere(*sh).is_null() == false);

      // Cleaner code helpers
      typedef std::vector<Object_3> Intersection_list;
      typedef std::pair<Circular_arc_point_3, unsigned int> CAP;
      const Sphere_3 & sphere = *sh;
      const Point_3 & sphere_center = sh->center();

      // Normal event sites, ordered by corresponding point
      typedef Normal_event_site<Kernel> NES;
      typedef std::map<Circular_arc_point_3, NES> NES_map;
      NES_map nes_map;

#define ADD_IE_TO_SITE(POINT, TYPE)                              \
      { typedef std::pair<const Circular_arc_point_3, NES> pair; \
        typename NES_map::iterator it = nes_map.find(POINT);     \
        if (it == nes_map.end())                                 \
        { it = nes_map.insert(std::make_pair(POINT,              \
              NES(sh, POINT))).first; }                          \
        it->second.add_event(IE(c1, c2, IE::TYPE)); }

      // Get the sphere's circles
      typedef std::vector<Circle_handle> Circle_list;
      Circle_list circle_list;
      _si->circles_on_sphere(sh, circle_list.begin());

      // Store the line passing through the poles
      // FIXME ordering north/south is not handled, but set arbitrarily
      Line_3 pole_axis(sh->center(), _axis_dir);
      CAP poles[2];
      Intersection_list poles_found;
      Intersect_3()(pole_axis, sphere, std::back_inserter(poles_found));
      CGAL_assertion(poles_found.size() == 2);
      POSSIBLY_ASSERT(Assign_3()(poles[0], poles_found[0]));
      POSSIBLY_ASSERT(Assign_3()(poles[1], poles_found[1]));
      Circular_arc_point_3 north(poles[0].first), south(poles[1].first);

      // Redundant code
#define ASSIGN_POLE_TO_CAP(OBJ, CAP)           \
      { POSSIBLY_ASSERT(Assign_3()(CAP, OBJ)); \
      /*CGAL_assertion(CAP.first == north      \
          || CAP.first == south);*/ } // FIXME

      for (typename Circle_list::const_iterator it = circle_list.begin();
          it != circle_list.end(); it++)
      {
        // Cleaner code
        const Circle_handle & ch1 = *it;
        const Circle_3 & c1 = *ch1;

        // Make polar/bipolar events
        Intersection_list intersections_at_poles;
        Intersect_3()(c1, pole_axis,
            std::back_inserter(intersections_at_poles));
        if (intersections_at_poles.size() > 0)
        {
          if (intersections_at_poles.size() == 1)
          {
            CAP cap;
            ASSIGN_POLE_TO_CAP(intersections_at_poles[0], cap);
            // TODO add polar event
          }
          else
          {
            CGAL_assertion(intersections_at_poles.size() == 2);
            CAP caps[2];
            ASSIGN_POLE_TO_CAP(intersections_at_poles[0], caps[0]);
            ASSIGN_POLE_TO_CAP(intersections_at_poles[1], caps[1]);
            // TODO add bipolar event
          }
        }
#undef ASSIGN_POLE_TO_CAP

        // Make normal start/end events
        // TODO find start/end points on each circle

        // Make crossing/tangency events
        for (typename Circle_list::const_iterator it2 = it + 1;
            it2 != circle_list.end(); it2++)
        {
          // *More* syntaxic sugar
          typedef Intersection_event<Kernel> IE;
          const Circle_handle & ch2 = *it2;
          const Circle_3 & c2 = *ch2;

          // Intersection circles must be different
          CGAL_assertion(ch1 != ch2 && c1 != c2);

          // Do intersections
          Intersection_list circle_intersections;
          Intersect_3()(c1, c2, std::back_inserter(circle_intersections));

          // Handle intersections
          if (circle_intersections.empty())
          { continue; }
          else if (circle_intersections.size() == 1) // Equality or Tangency
          {
            // Test if intersection is a point -> tangency
            CAP cap;
            if (Assign_3()(cap, circle_intersections[0]))
            {
              // Handle circle tangency
              ADD_IE_TO_SITE(cap.first, Tangency);
              continue;
            }

            // Intersection is necessarily a circle
            Circle_3 c;
            POSSIBLY_ASSERT(Assign_3()(c, circle_intersections[0]));
            //handle_circle_equality(ev_queue, c1, c2); FIXME
          }
          else // Crossing
          {
            // There is necessarily two intersections
            CGAL_assertion(circle_intersections.size() == 2);

            CAP cap1, cap2;
            POSSIBLY_ASSERT(Assign_3()(cap1, circle_intersections[0]));
            POSSIBLY_ASSERT(Assign_3()(cap2, circle_intersections[1]));
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

      // Now that the normal events are all regrouped in event sites,
      // add all the event sites to the event queue
      for (typename NES_map::const_iterator it = nes_map.begin();
          it != nes_map.end(); it++)
      { ev_queue.push(it->second); }

      return ev_queue;
    }

  private:
    const SI * _si;
    Direction_3 _axis_dir;
};

#endif // EVENT_QUEUE_BUILDER_H // vim: sw=2 et ts=2 sts=2
