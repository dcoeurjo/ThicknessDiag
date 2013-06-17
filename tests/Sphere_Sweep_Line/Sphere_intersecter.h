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
  typedef typename Kernel::Sphere_3 Sphere_3;
  typedef typename Kernel::Circle_3 Circle_3;
  typedef typename Kernel::Circular_arc_3 Circular_arc_3;
  typedef typename Kernel::Circular_arc_point_3 Circular_arc_point_3;

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

    struct Comp_sphere_handle_pair
    {
      typedef std::pair<Sphere_handle, Sphere_handle> Sphere_handle_pair;

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
    typedef std::map<std::pair<Sphere_handle, Sphere_handle>,
            Circle_handle, Comp_sphere_handle_pair> Circle_intersects;

  public:
    Sphere_handle add_sphere(const Sphere_3 & s)
    {
      // Shortcut
      typedef typename Sphere_list::iterator Sphere_iterator;

#ifndef NDEBUG // Check assertion
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
      Sphere_iterator it = _sphere_list.insert(s);
      // TODO compute intersections
      return Sphere_handle(*it);
    }

    template <typename InputIterator>
    void add_sphere(InputIterator begin, InputIterator end)
    {
      for (; begin != end; begin++)
      { add_sphere(*begin); }
    }

  private:
    Sphere_list _sphere_list;
    Circle_list _circle_list;
};

#endif // SPHERE_INTERSECTER_H // vim: sw=2 et ts=2 sts=2
