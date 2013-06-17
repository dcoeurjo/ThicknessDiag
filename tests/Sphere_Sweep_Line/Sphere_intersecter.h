#ifndef SPHERE_INTERSECTER_H
#define SPHERE_INTERSECTER_H

#include <set>
#include <map>
#include <algorithm>
#include <Spherical_utils.h>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

template <typename Kernel>
class Sphere_intersecter
{
  // Geometric objects bundle
  typedef typename Kernel::Point_3 Point_3;
  typedef typename Kernel::Sphere_3 Sphere_3;
  typedef typename Kernel::Circle_3 Circle_3;
  typedef typename Kernel::Circular_arc_3 Circular_arc_3;
  typedef typename Kernel::Circular_arc_point_3 Circular_arc_point_3;

  // Intersection bundle
  typedef typename Kernel::Object_3 Object_3;
  typedef typename Kernel::Assign_3 Assign_3;
  typedef typename Kernel::Intersect_3 Intersect_3;

  // Similar to boost::reference_wrapper (follows the
  // same interface as this one). The only difference
  // is that we don't want objects of this type to
  // be constructed otherwise than via the main class.
  template <typename T>
  class Handle
  {
    public:
      typedef T type;

      const type & get() const
      { return _impl; }

      const type * get_pointer() const
      { return &_impl; }

      operator type& () const 
      { return _impl; }

    private:
      friend class Sphere_intersecter<Kernel>;

      Handle(const type & i):
        _impl(i) {}

      const type & _impl;
  };

  public:
    typedef Handle<Sphere_3> Sphere_handle;
    typedef Handle<Circle_3> Circle_handle;

  private:
    template <typename Handle>
    struct Comp_handle_ptr
    {
      bool operator()(const Handle & x,
          const Handle & y) const
      {
        return x.get_pointer() < y.get_pointer();
      }
    };

    typedef std::pair<Sphere_handle, Sphere_handle> Sphere_handle_pair;

    struct Comp_sphere_handle_pair
    {
      bool operator()(const Sphere_handle_pair & x,
          const Sphere_handle_pair & y) const
      {
        Comp_handle_ptr<Sphere_handle> comp_handle;
        return comp_handle(x.first, y.first)
          || (!comp_handle(y.first, x.first)
              && comp_handle(x.second, y.second));
      }
    };

    // List of actual geometrical objects
    typedef std::multiset<Sphere_3,
            Comp_by_squared_radii<Sphere_3> > Sphere_list;
    typedef std::multiset<Circle_3,
            Comp_by_squared_radii<Circle_3> > Circle_list;

    // Link between a sphere intersection and the source spheres
    typedef std::map<Sphere_handle_pair, Circle_handle,
            Comp_sphere_handle_pair> Circle_intersects;

  public:
    Sphere_handle add_sphere(const Sphere_3 & s)
    { return setup_new_sphere(s); }

    template <typename InputIterator>
    void add_sphere(InputIterator begin, InputIterator end)
    {
      for (; begin != end; begin++)
      { add_sphere(*begin); }
    }

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
      for (Sphere_iterator it = _sphere_list.begin();
          it != _sphere_list.end(); it++)
      {
        // Syntaxic sugar
        const Sphere_3 & s1 = sh;
        const Sphere_3 & s2 = *it;

        // Try intersection
        Object_3 obj = Intersect_3()(s1, s2);

        // Handle empty intersection
        if (obj.is_empty())
        { continue; }

        // Assign function object
        Assign_3 assign;

#if 0 && !defined(NDEBUG)
        if (const Sphere_3 * sp = object_cast<Sphere_3>(&obj))
        {
          std::ostringstream oss;
          oss << "Forbidden intersection between two"
            << " equal spheres " << *sp;
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
      Sphere_handle_pair p(Sphere_handle(s1), Sphere_handle(s2));
#ifndef NDEBUG
      // TODO
#endif // NDEBUG //
      Circle_handle ch(*_circle_list.insert(c));
      _circle_intersects[p] = ch;
      return ch;
    }

    Sphere_list _sphere_list;
    Circle_list _circle_list;

    Circle_intersects _circle_intersects;
};

#endif // SPHERE_INTERSECTER_H // vim: sw=2 et ts=2 sts=2
