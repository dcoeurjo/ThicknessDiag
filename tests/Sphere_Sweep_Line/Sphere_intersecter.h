#ifndef SPHERE_INTERSECTER_H
#define SPHERE_INTERSECTER_H

#include <map>
#include <vector>
#include <iterator>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

#include <boost/utility.hpp>
#ifdef PROFILING_MODE
#  include <string>
#  include <iostream>
#  include <boost/timer.hpp>
#  define PROFILE_SCOPE_WITH_NAME(s) priv::profiling_timer t(s)

namespace priv {
  class profiling_timer
  {
    public:
      profiling_timer(const std::string & name):
        _name(name), _timer() {}

      ~profiling_timer()
      { std::cout << _name << " "
        << _timer.elapsed() << std::endl; }

    private:
      const std::string _name;
      boost::timer _timer;
  };
}

#else
#  define PROFILE_SCOPE_WITH_NAME(s)
#endif // PROFILING_MODE //

#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>

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

  // Friend access
  friend class Sphere_iterator;
  friend class Sphere_iterator_range;

  // AABB primitive for tree usage
  template <typename T>
  class AABB_handle_primitive;

  // Similar to boost::reference_wrapper. The only
  // difference lies in the fact that we don't want
  // objects of this type to be constructed otherwise
  // than via the main class (better encapsulation).
  template <typename T>
  class Handle
  {
    friend class Sphere_intersecter<Kernel>;
    friend class AABB_handle_primitive<T>;

    public:
      typedef T Type;

      Handle():
        _t(0) {}

      const Type * get_pointer() const
      { return _t; }

      Type & get() const
      { return *_t; }

      operator Type& () const
      { return get(); }

    private:
      Handle(Type & t):
        _t(&t) {}

      Type * _t;
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

    // AABB Tree primitive for locating spheres
    template <typename T>
    class AABB_handle_primitive
    {
      public:
        typedef Point_3 Point;
        typedef Handle<const T> Id;
        typedef typename Id::Type & Datum;

        AABB_handle_primitive(Datum d):
          _handle(d) {}

        Datum datum() const
        { return _handle.get(); }

        Id id() const
        { return _handle; }

        Point reference_point() const
        { BOOST_AUTO(bbox, _handle.get().bbox());
          return Point(bbox.xmin(), bbox.ymin(), bbox.zmin()); }

      private:
        Id _handle;
    };

    // AABB tree of spheres
    typedef AABB_handle_primitive<Sphere_3> AABB_sphere_primitive;
    typedef CGAL::AABB_tree<CGAL::AABB_traits<Kernel,
            AABB_sphere_primitive> > Sphere_tree;

    // Actual list of spheres (used only for storage)
    typedef std::list<Sphere_3> Sphere_storage;
    // ...same for circles
    typedef std::list<Circle_3> Circle_storage;
    // ...and for arcs
    // TODO
    // Note that we cannot use a vector since the address should remain
    // the same after the first insertion

    // Templated typedef for mapping from handle to something (major
    // refactoring for links)
    template <typename Handle, typename Mapped>
    struct Handle_map
    { typedef std::map<Handle, Mapped,
              Comp_handle_ptr<Handle> > Type; };

    // Link between a sphere intersection and the source spheres
    // ...sphere -> circle link (single)
    typedef typename Handle_map<Sphere_handle,
            Circle_handle>::Type Sphere_to_circle_single_link;
    // .. sphere -> sphere -> circle link (double)
    typedef typename Handle_map<Sphere_handle,
            Sphere_to_circle_single_link>::Type Spheres_to_circle_link;

    // Link between a circle and the spheres intersecting
    typedef std::pair<Sphere_handle, Sphere_handle> Sphere_handle_pair;
    typedef typename Handle_map<Circle_handle,
            Sphere_handle_pair>::Type Circle_to_spheres_link;

  public:
    // Setup new sphere
    Sphere_handle add_sphere(const Sphere_3 & s)
    { return new_sphere(s); }

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

        Sphere_insert_iterator(Sphere_intersecter<Kernel> & si):
          _si(si) {}

        Sphere_intersecter<Kernel> & _si;
    };

    Sphere_insert_iterator insert_iterator()
    { return Sphere_insert_iterator(*this); }

    class Sphere_iterator_range;

    class Sphere_iterator:
      public std::iterator<std::input_iterator_tag, Sphere_handle>
    {
      // Friend access
      friend class Sphere_iterator_range;

      // Internal shortcuts
      typedef typename Sphere_storage::const_iterator real_iterator;

      public:
        Sphere_iterator & operator++()
        { ++_it; return *this; }

        Sphere_iterator operator++(int)
        { Sphere_iterator tmp(*this);
          ++(*this); return tmp; }

        bool operator==(const Sphere_iterator & sit) const
        { return _it == sit._it; }

        bool operator!=(const Sphere_iterator & sit) const
        { return !(*this == sit); }

        Sphere_handle operator*()
        { return Sphere_handle(*_it); }

      private:
        Sphere_iterator(const real_iterator & it):
          _it(it) {}

        real_iterator _it;
    };

    class Sphere_iterator_range
    {
      // Friend access
      friend class Sphere_intersecter<Kernel>;

      public:
        Sphere_iterator begin() const
        { return Sphere_iterator(_si._sphere_storage.begin()); }

        Sphere_iterator end() const
        { return Sphere_iterator(_si._sphere_storage.end()); }

      private:
        Sphere_iterator_range(const Sphere_intersecter<Kernel> & si):
          _si(si) {}

        const Sphere_intersecter<Kernel> & _si;
    };

    Sphere_iterator_range spheres() const
    { return Sphere_iterator_range(*this); }

  private:
    // Insert sphere and setup intersection links.
    // Precondition: sphere isn't yet inserted
    Sphere_handle new_sphere(const Sphere_3 & sphere_to_insert)
    {
      PROFILE_SCOPE_WITH_NAME("Sphere_intersecter::new_sphere");

      // Store a copy of the inserted sphere and insert
      // a handle of this inserted sphere in the Tree
      _sphere_storage.push_back(sphere_to_insert);
      const Sphere_3 & s1 = _sphere_storage.back();
      _sphere_tree.insert(AABB_sphere_primitive(s1));
      Sphere_handle sh1(s1);

      // No need to test for intersections when there is only one element
      if (_sphere_tree.size() > 1)
      {
        // Find intersected balls
        std::vector<Sphere_handle> intersected;
        _sphere_tree.all_intersected_primitives(s1,
            std::back_inserter(intersected));

        // Compute intersections
        for (typename std::vector<Sphere_handle>::const_iterator
            it = intersected.begin(); it != intersected.end(); it++)
        {
          // Syntaxic sugar
          const Sphere_3 & s2 = *it;

          // Handles for the spheres
          handle_sphere_intersection(sh1, Sphere_handle(s2));
        }
      }
      return sh1;
    }

    void handle_sphere_intersection(const Sphere_handle & sh1,
        const Sphere_handle & sh2)
    {
      PROFILE_SCOPE_WITH_NAME("Sphere_intersecter::handle_sphere_intersection");

      // More syntaxic sugar
      typename Sphere_handle::Type & s1 = sh1.get();
      typename Sphere_handle::Type & s2 = sh2.get();

      // Intersection function objects
      Intersect_3 intersection;
      Assign_3 assign;

      // Skip inserted sphere (comparing by address is *much* faster,
      // and valid since we only store the sphere once, and pass around
      // handles holding a const reference to this sphere)
      if (&s1 == &s2)
      { return; }

      // Try intersection
      Object_3 obj = intersection(s1, s2);

      // Intersection along a circle
      Circle_3 c;
      if (assign(c, obj))
      { new_sphere_intersection(sh1, sh2, c);
        return; }

      // Intersection along a point
      Point_3 p;
      if (assign(p, obj))
      { new_sphere_intersection(sh1, sh2, Circle_3(p, 0,
            Line_3(s1.center(), s2.center()
              ).perpendicular_plane(p)));
      return; }

#ifndef NDEBUG // Check edge case of intersection along the same sphere
      Sphere_3 sp;
      if (assign(sp, obj))
      {
        std::ostringstream oss;
        oss << "Forbidden intersection between two"
          << " equal spheres " << sp;
        throw std::runtime_error(oss.str());
      }
#endif // NDEBUG //
    }

    // Preconditions:
    //  - this particular intersection hasn't been
    //    inserted until now (checked only for sphere -> circle link)
    void new_sphere_intersection(const Sphere_handle & sh1,
        const Sphere_handle & sh2, const Circle_3 & c)
    {
      PROFILE_SCOPE_WITH_NAME("Sphere_intersecter::new_sphere_intersection");

      // Store the circle of intersection
      _circle_storage.push_back(c);
      const Circle_3 & ch(_circle_storage.back());

      // Prepare the links
      BOOST_AUTO(sh1_link, _stcl[sh1]);
      BOOST_AUTO(sh2_link, _stcl[sh2]);

#ifndef NDEBUG // Check preconditions
      if (sh1_link.find(sh2) != sh1_link.end()
          || sh2_link.find(sh1) != sh2_link.end())
      {
        std::ostringstream oss;
        oss << "Forbidden second insertion of the same sphere intersection"
          << " between " << sh1.get() << " and " << sh2.get();
        throw std::runtime_error(oss.str());
      }
#endif // NDEBUG //

      // Setup the links
      sh1_link[sh2] = ch;
      sh2_link[sh1] = ch;
      _ctsl[ch] = Sphere_handle_pair(sh1, sh2);

      // Compute the intersections between circles on each sphere
      new_circle_on_sphere(sh1, ch);
      new_circle_on_sphere(sh2, ch);
    }

    void new_circle_on_sphere(const Sphere_handle & sh,
        const Circle_handle & ch)
    {
      PROFILE_SCOPE_WITH_NAME("Sphere_intersecter::new_circle_on_sphere");
    }

    // Sphere bundle
    Sphere_tree _sphere_tree;
    Sphere_storage _sphere_storage;

    // Circle bundle
    Circle_storage _circle_storage;

    // Spheres <-> Circles
    Spheres_to_circle_link _stcl;
    Circle_to_spheres_link _ctsl;
};

#endif // SPHERE_INTERSECTER_H // vim: sw=2 et ts=2 sts=2
