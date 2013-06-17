#ifndef SPHERE_INTERSECTER_H
#define SPHERE_INTERSECTER_H

#include <set>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

#include <boost/ref.hpp>

#include <Spherical_utils.h>

template <typename Kernel>
class Sphere_intersecter
{
  // Geometric objects bundle
  typedef typename Kernel::Point_3 Point_3;
  typedef typename Kernel::Sphere_3 Sphere_3;
  typedef typename Kernel::Circle_3 Circle_3;
  typedef typename Kernel::Line_3 Line_3;
  typedef typename Kernel::Circular_arc_3 Circular_arc_3;
  typedef typename Kernel::Circular_arc_point_3 Circular_arc_point_3;

  // Intersection bundle
  typedef typename Kernel::Object_3 Object_3;
  typedef typename Kernel::Assign_3 Assign_3;
  typedef typename Kernel::Intersect_3 Intersect_3;

  // Internal shortcuts for readability
  typedef Sphere_intersecter<Kernel> Self;

  // Extension of boost::reference_wrapper. The only
  // difference lies in the fact that we don't want
  // objects of this type to be constructed otherwise
  // than via the main class (better encapsulation).
  template <typename T>
  class Handle: public boost::reference_wrapper<T>
  {
    friend class Sphere_intersecter<Kernel>;

    Handle(T & t):
      boost::reference_wrapper<T>(t) {}
  };

  public:
    typedef Handle<const Sphere_3> Sphere_handle;
    typedef Handle<const Circle_3> Circle_handle;

  private:
    template <typename Handle>
    struct Comp_handle_ptr
    {
      bool operator()(const Handle & x,
          const Handle & y) const
      { return &x < &y; }
    };

    typedef Comp_handle_ptr<Sphere_handle> Comp_sphere_handle_ptr;

    // List of actual geometrical objects
    typedef std::multiset<Sphere_3,
            Comp_by_squared_radii<Sphere_3> > Sphere_list;
    typedef std::multiset<Circle_3,
            Comp_by_squared_radii<Circle_3> > Circle_list;

    // Link between a sphere intersection and the source spheres
    typedef std::map<Sphere_handle, std::map<Sphere_handle,
            Circle_handle, Comp_sphere_handle_ptr>,
            Comp_sphere_handle_ptr> Circle_intersects;

    // Link between a sphere and the circles on it
    typedef std::map<Sphere_handle, std::vector<Circle_handle>,
            Comp_sphere_handle_ptr> Circles_on_sphere;

  public:
    Sphere_handle add_sphere(const Sphere_3 & s)
    { return setup_new_sphere(s); }

    template <typename InputIterator>
    void add_sphere(InputIterator begin, InputIterator end)
    {
      for (; begin != end; begin++)
      { add_sphere(*begin); }
    }

    class Sphere_insert_iterator:
      public std::iterator<std::output_iterator_tag,
      void, void, void, void>
    {
      public:
        Sphere_insert_iterator & operator=(const Sphere_3 & s)
        { _si.add_sphere(s); 
        return *this; }

        Sphere_insert_iterator & operator++()
        { return *this; }
        Sphere_insert_iterator & operator++(int)
        { return *this; }

        Sphere_insert_iterator & operator*()
        { return *this; }

      private:
        friend class Sphere_intersecter<Kernel>;

        Sphere_insert_iterator(Self & si):
          _si(si) {}

        Self & _si;
    };

    Sphere_insert_iterator insert_iterator()
    { return Sphere_insert_iterator(*this); }

  private:
    // Insert sphere and setup intersection links.
    // Precondition: sphere isn't yet inserted
    Sphere_handle setup_new_sphere(const Sphere_3 & s)
    {
      // Shortcut
      typedef typename Sphere_list::const_iterator Sphere_iterator;

#ifndef NDEBUG // Check precondition
      std::pair<Sphere_iterator, Sphere_iterator> it_range;
      it_range = _sphere_list.equal_range(s);
      if (std::find(it_range.first, it_range.second, s) != it_range.second)
      {
        std::ostringstream oss;
        oss << "Concept violation, cannot insert twice the same"
          << " sphere in the sphere proxy" << std::endl
          << "Found range:" << std::endl;
        for (Sphere_iterator it = it_range.first;
            it != it_range.second; it++)
        {
          oss << "  " << *it << std::endl;
        }
        oss << std::endl
          << "Equal to: " << s << std::endl;
        throw std::logic_error(oss.str());
      }
#endif // NDEBUG //

      // Insert the sphere (needed here for compare by address)
      Sphere_handle sh(*_sphere_list.insert(s));

      // Compute its intersections
      // TODO use AABB Tree
      for (Sphere_iterator it = _sphere_list.begin();
          it != _sphere_list.end(); it++)
      {
        // Syntaxic sugar
        const Sphere_3 & s1 = sh.get();
        const Sphere_3 & s2 = *it;

        // Try intersection
        Object_3 obj = Intersect_3()(s1, s2);

        // Handle empty intersection
        if (obj.is_empty())
        { continue; }

        // Assign function object
        Assign_3 assign;

#ifndef NDEBUG // Check precondition
        Sphere_3 sp;
        if (assign(sp, obj))
        {
          std::ostringstream oss;
          oss << "Forbidden intersection between two"
            << " equal spheres " << sp;
          throw std::runtime_error(oss.str());
        }
#endif // NDEBUG //

        // Intersection along a circle
        Circle_3 c;
        if (assign(c, obj))
        { setup_new_circle(s1, s2, c);
          continue; }

        // Intersection along a point
        Point_3 p;
        if (assign(p, obj))
        { setup_new_circle(s1, s2, Circle_3(p, 0,
              Line_3(s1.center(), s2.center()
                ).perpendicular_plane(p)));
          continue; }
      }

      return sh;
    }

    // Precondition: this particular intersection hasn't been
    // inserted until now.
    Circle_handle setup_new_circle(const Sphere_3 & s1,
        const Sphere_3 & s2,
        const Circle_3 & c)
    {
      Sphere_handle sh1(s1), sh2(s2);
#ifndef NDEBUG
      // TODO check precondition
#endif // NDEBUG //
      Circle_handle ch(*_circle_list.insert(c));
      _circle_intersects[sh1].insert(std::make_pair(sh2, ch));
      _circle_intersects[sh2].insert(std::make_pair(sh1, ch));
      setup_circle_intersections_on_sphere(s1, ch.get());
      setup_circle_intersections_on_sphere(s2, ch.get());
      return ch;
    }

    void setup_circle_intersections_on_sphere(const Sphere_3 & s,
        const Circle_3 & c)
    {
      _circles_on_sphere[Sphere_handle(s)].push_back(Circle_handle(c));
      // TODO handle intersection between circles
    }

    Sphere_list _sphere_list;
    Circle_list _circle_list;
    Circle_intersects _circle_intersects;
    Circles_on_sphere _circles_on_sphere;
};

#endif // SPHERE_INTERSECTER_H // vim: sw=2 et ts=2 sts=2
