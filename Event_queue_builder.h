#ifndef EVENT_QUEUE_BUILDER_H
#define EVENT_QUEUE_BUILDER_H

#include <vector>
#include <utility>
#include <iterator>

#include <CGAL/assertions.h>
#include <CGAL/Kernel/global_functions_3.h>

#ifndef NDEBUG
#  define POSSIBLY_ASSERT(S) CGAL_assertion(S);
#else
#  define POSSIBLY_ASSERT(S) S;
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
    EQ operator()(const SI & si, const Sphere_3 & s)
    { Sphere_handle sh = si.find_sphere(s);
      return (*this)(si, sh); }

    EQ operator()(const SI & si, const Sphere_handle & sh)
    {
      // Check basic assertions
      CGAL_assertion(sh.is_null() == false);
      CGAL_assertion(si.find_sphere(*sh).is_null() == false);

      // Cleaner code helpers
      typedef Polar_event<Kernel> PE;
      typedef Polar_event_site<Kernel> PE_site;
      typedef Normal_event<Kernel> NE;
      typedef Normal_event_site<Kernel> NE_site;
      typedef std::vector<Object_3> Intersection_list;
      typedef std::pair<Circular_arc_point_3, unsigned int> CAP;
      const Sphere_3 & sphere = *sh;

      // Get the sphere's circles
      typedef std::vector<Circle_handle> Circle_list;
      Circle_list circle_list;
      si.circles_on_sphere(sh, std::back_inserter(circle_list));

      // Store the line passing through the poles
      Line_3 pole_axis(sh->center(), Direction_3(0, 0, 1));
      CAP poles[2];
      Intersection_list poles_found;
      Intersect_3()(pole_axis, sphere, std::back_inserter(poles_found));
      CGAL_assertion(poles_found.size() == 2);
      POSSIBLY_ASSERT(Assign_3()(poles[0], poles_found[0]));
      POSSIBLY_ASSERT(Assign_3()(poles[1], poles_found[1]));
      Circular_arc_point_3 north, south;
      // Minmax along Z to find the north/south
      {
        using namespace CGAL;
        const Circular_arc_point_3 & a = poles[0].first;
        const Circular_arc_point_3 & b = poles[1].first;
        const Comparison_result comp_res = compare_z(a, b);
        if (comp_res == SMALLER)
        {
          north = b;
          south = a;
        }
        else
        {
          CGAL_assertion(comp_res == LARGER);
          north = a;
          south = b;
        }
        CGAL_assertion(compare_z(south, north) == SMALLER);
      }
      // ...helper macro for redundant code
#define ASSIGN_POLE_TO_CAP(OBJ, CAP)           \
      { POSSIBLY_ASSERT(Assign_3()(CAP, OBJ)); \
      CGAL_assertion(CAP.first == north      \
          || CAP.first == south); } // FIXME

      // Normal event sites, ordered by corresponding point
      typedef std::map<Circular_arc_point_3, NE_site> NE_site_map;
      NE_site_map ne_site_map;
      // ...helper macro for redundant code
#define ADD_NE_TO_NE_SITE(POINT, TYPE)                               \
      { typedef std::pair<const Circular_arc_point_3, NE_site> pair; \
        typename NE_site_map::iterator it = ne_site_map.find(POINT); \
        if (it == ne_site_map.end())                                 \
        { it = ne_site_map.insert(std::make_pair(POINT,              \
              NE_site(sh, POINT))).first; }                          \
        it->second.add_event(IE(c1, c2, IE::TYPE)); }

      // Polar event sites, 3 exactly:
      //  * north pole
      //  * south pole
      //  * north / south bipole
      // TODO

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
        else
        {
          // Make normal start/end events
          // TODO find start/end points on each circle
        }

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
              ADD_NE_TO_NE_SITE(cap.first, Tangency);
              continue;
            }

            // Intersection is necessarily a circle
            Circle_3 c;
            POSSIBLY_ASSERT(Assign_3()(c, circle_intersections[0]));
            //handle_circle_equality(c1, c2); FIXME
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
            ADD_NE_TO_NE_SITE(cap1.first, Largest_crossing);
            ADD_NE_TO_NE_SITE(cap1.first, Smallest_crossing);
            // ...second point
            ADD_NE_TO_NE_SITE(cap2.first, Largest_crossing);
            ADD_NE_TO_NE_SITE(cap2.first, Smallest_crossing);
          }
        }
      }
#undef ADD_NE_TO_NE_SITE

      EQ ev_queue;

      // Now that the normal events are all regrouped in event sites,
      // add all the event sites to the event queue
      for (typename NE_site_map::const_iterator it = ne_site_map.begin();
          it != ne_site_map.end(); it++)
      { ev_queue.push(it->second); }

      return ev_queue;
    }
};

#endif // EVENT_QUEUE_BUILDER_H // vim: sw=2 et ts=2 sts=2
