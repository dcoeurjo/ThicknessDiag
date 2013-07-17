#ifndef EVENT_QUEUE_BUILDER_H
#define EVENT_QUEUE_BUILDER_H

#include <vector>
#include <utility>
#include <iterator>
#include <algorithm>

#include <CGAL/assertions.h>
#include <CGAL/Kernel/global_functions_3.h>
#include <CGAL/Spherical_kernel_3.h>
#include <CGAL/global_functions_spherical_kernel_3.h>

#include <Event_queue.h>
#include <Sphere_intersecter.h>

template <typename SK>
class Event_queue_builder
{
  // Geometrical objects
  typedef typename SK::Line_3 Line_3;
  typedef typename SK::Point_3 Point_3;
  typedef typename SK::Plane_3 Plane_3;
  typedef typename SK::Circle_3 Circle_3;
  typedef typename SK::Sphere_3 Sphere_3;
  typedef typename SK::Vector_3 Vector_3;
  typedef typename SK::Segment_3 Segment_3;
  typedef typename SK::Direction_3 Direction_3;
  typedef typename SK::Circular_arc_3 Circular_arc_3;
  typedef typename SK::Circular_arc_point_3 Circular_arc_point_3;

  // Function objects
  typedef typename SK::Object_3 Object_3;
  typedef typename SK::Assign_3 Assign_3;
  typedef typename SK::Intersect_3 Intersect_3;
  typedef typename SK::Compare_theta_3 Compare_theta_3;
  typedef typename SK::Compare_theta_z_3 Compare_theta_z_3;

  // Sphere intersecter and related
  typedef Sphere_intersecter<SK> SI;
  typedef typename SI::Sphere_handle Sphere_handle;
  typedef typename SI::Circle_handle Circle_handle;

  // Event queue, events and event sites
  typedef Event_queue<SK> EQ;
  typedef typename EQ::Events Events;
  typedef typename Events::Polar_event Polar_event;
  typedef typename Events::Critical_event Critical_event;
  typedef typename Events::Bipolar_event Bipolar_event;
  typedef typename Events::Intersection_event Intersection_event;
  typedef typename Events::Polar_event_site Polar_event_site;
  typedef typename Events::Normal_event_site Normal_event_site;
  typedef typename Events::Bipolar_event_site Bipolar_event_site;
  // ...event builders
  typedef typename Events::Event_builder Event_builder;
  typedef typename Events::Circle_event_builder Circle_event_builder;

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
      typedef std::pair<Circular_arc_point_3, unsigned int> CAP;
      typedef std::vector<Object_3> Intersection_list;
      const Sphere_3 & sphere = *sh;

      // Get the sphere's circles
      typedef std::vector<Circle_handle> Circle_list;
      Circle_list circle_list;
      si.circles_on_sphere(sh, std::back_inserter(circle_list));

      // Normal event sites, mapped by corresponding point
      typedef std::map<Circular_arc_point_3,
              Normal_event_site> Normal_event_sites;
      Normal_event_sites normal_sites;
      // ...helper macro for redundant code
#define ADD_TO_NE_SITE(POINT, EVENT)                                         \
      { typename Normal_event_sites::iterator it = normal_sites.find(POINT); \
        if (it == normal_sites.end())                                        \
        { it = normal_sites.insert(std::make_pair(POINT,                     \
              Normal_event_site(sh, POINT))).first; }                        \
        it->second.add_event(EVENT); }

      // Polar event sites
      typedef std::vector<Polar_event_site> Polar_event_sites;
      Polar_event_sites pe_sites;

      // Event builder for this sphere
      Event_builder eb(*sh);

      for (typename Circle_list::const_iterator it = circle_list.begin();
          it != circle_list.end(); it++)
      {
        // Cleaner code
        const Circle_handle & ch1 = *it;
        const Circle_3 & c1 = *ch1;

        // Add circles events
        CGAL::Circle_type circle_type = CGAL::classify(c1, *sh);
        if (circle_type != CGAL::THREADED)
        {
          Circle_event_builder ceb = eb.prepare_circle_event(c1);
          if (circle_type == CGAL::NORMAL)
          {
            Circular_arc_point_3 extremes[2];
            CGAL::theta_extremal_points(c1, *sh, extremes);
            ADD_TO_NE_SITE(extremes[0], ceb.critical_event(extremes[0], Critical_event::Start));
            ADD_TO_NE_SITE(extremes[1], ceb.critical_event(extremes[1], Critical_event::End));
          }
          else if (circle_type == CGAL::POLAR)
          {
            // Line passing through the poles
            Line_3 pole_axis(sh->center(), Direction_3(0, 0, 1));

            // Compute intersection
            Intersection_list pole_inters;
            Intersect_3()(pole_axis, c1, std::back_inserter(pole_inters));
            CGAL_assertion(pole_inters.size() == 1);
            CAP cap;
            Assign_3()(cap, pole_inters[0]);
            typename Polar_event::Pole_type pole_type = Polar_event::North;
            if (CGAL::compare_z(Circular_arc_point_3(sh->center()), cap.first) == CGAL::SMALLER)
            { pole_type = Polar_event::South; }

            // Add polar events
            pe_sites.push_back(Polar_event_site(ceb.polar_event(cap.first, pole_type, Polar_event::Start)));
            pe_sites.push_back(Polar_event_site(ceb.polar_event(cap.first, pole_type, Polar_event::End)));
          }
          else
          {
            CGAL_assertion(circle_type == CGAL::BIPOLAR);
            Vector_3 circle_normal = c1.supporting_plane().orthogonal_vector();
            Vector_3 meridian_normals[2] = { circle_normal, -circle_normal };
            std::sort(meridian_normals, meridian_normals + 2, Compare_theta_3(*sh));
            // TODO
          }
        }

        // Make crossing/tangency events
        for (typename Circle_list::const_iterator it2 = it + 1;
            it2 != circle_list.end(); it2++)
        {
          // *More* syntaxic sugar
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
              ADD_TO_NE_SITE(cap.first, eb.intersection_event(ch1, ch2, cap.first, Intersection_event::Tangency));
              continue;
            }

            // Intersection is necessarily a circle
            Circle_3 c;
            Assign_3()(c, circle_intersections[0]);
            // FIXME
          }
          else // Crossing
          {
            // There is necessarily two intersections
            CGAL_assertion(circle_intersections.size() == 2);

            CAP cap1, cap2;
            Assign_3()(cap1, circle_intersections[0]);
            Assign_3()(cap2, circle_intersections[1]);
            CGAL_assertion(cap1.second == 1 && cap2.second == 1);

            // Handle circle crossing
            // ...first point
            ADD_TO_NE_SITE(cap1.first, eb.intersection_event(ch1, ch2, cap1.first, Intersection_event::Largest_crossing));
            ADD_TO_NE_SITE(cap1.first, eb.intersection_event(ch1, ch2, cap1.first, Intersection_event::Smallest_crossing));
            // ...second point
            ADD_TO_NE_SITE(cap2.first, eb.intersection_event(ch1, ch2, cap2.first, Intersection_event::Largest_crossing));
            ADD_TO_NE_SITE(cap2.first, eb.intersection_event(ch1, ch2, cap2.first, Intersection_event::Smallest_crossing));
          }
        }
      }

      // Final event queue to build
      EQ ev_queue;

      // Now that the normal events are all regrouped in event sites,
      // add all the event sites to the event queue
      for (typename Normal_event_sites::const_iterator it = normal_sites.begin();
          it != normal_sites.end(); it++)
      { ev_queue.push(it->second); }
      // ...same for polar events sites
      for (typename Polar_event_sites::const_iterator it = pe_sites.begin();
          it != pe_sites.end(); it++)
      { ev_queue.push(*it); }

      return ev_queue;
    }
};

#endif // EVENT_QUEUE_BUILDER_H // vim: sw=2 et ts=2 sts=2
