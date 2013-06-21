#ifndef CIRCLE_INTERSECTER_H
#define CIRCLE_INTERSECTER_H

#include <vector>
#include <utility>
#include <iterator>

#include <CGAL/assertions.h>

template <typename Kernel, typename Circle_traits>
class Circle_intersecter
{
  typedef typename Kernel::Circle_3 Circle_3;
  typedef typename Kernel::Object_3 Object_3;
  typedef typename Kernel::Assign_3 Assign_3;
  typedef typename Kernel::Intersect_3 Intersect_3;
  typedef typename Kernel::Circular_arc_point_3 Circular_arc_point_3;

  typedef typename Circle_traits::Tangency Tangency;
  typedef typename Circle_traits::Crossing Crossing;
  typedef typename Circle_traits::Equality Equality;

  public:
    void operator()(const Circle_3 & c1, const Circle_3 & c2) const
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
        { Tangency()(c1, c2, cap.first);
          return; }

        // Intersection is necessarily a circle
        Circle_3 c;
        CGAL_assertion(Assign_3()(c, intersected[0]));
        Equality()(c1, c2);
      }
      else // Crossing
      {
        // There is necessarily two intersections
        CGAL_assertion(intersected.size() == 2);

        std::pair<Circular_arc_point_3, unsigned int> cap1, cap2;
        CGAL_assertion(Assign_3()(cap1, intersected[0])
            && Assign_3()(cap2, intersected[1]));
        CGAL_assertion(cap1.second == 1 && cap2.second == 1);
        Crossing()(c1, c2, std::make_pair(cap1.first, cap2.first));
      }
    }
};

#endif // CIRCLE_INTERSECTER_H // vim: sw=2 et ts=2 sts=2
