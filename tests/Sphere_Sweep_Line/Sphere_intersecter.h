#ifndef SPHERE_INTERSECTER_H
#define SPHERE_INTERSECTER_H

#include <map>
#include <vector>
#include <memory>
#include <iterator>

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#  include <forward_list>
#  define LINKED_LIST std::forward_list
#  define INFER_AUTO(x, y) auto x(y)
#  define SHARED_PTR std::shared_ptr
#else
#  include <list>
#  include <boost/shared_ptr.hpp>
#  define LINKED_LIST std::list
#  define INFER_AUTO BOOST_AUTO
#  define SHARED_PTR boost::shared_ptr
#endif // __GXX_EXPERIMENTAL_CXX0X__ //

#ifndef NDEBUG
#  include <CGAL/assertions.h>
#  include <sstream>
#endif // NDEBUG //

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
  typedef typename Kernel::Do_intersect_3 Do_intersect_3;
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

      Type * get_pointer() const
      { return _t; }

      Type & get() const
      { return *_t; }

      operator Type&() const
      { return get(); }

      bool operator<(const Handle<T> & h) const
      { return _t < h._t; }

      bool operator==(const Handle<T> & h) const
      { return _t == h._t; }

    private:
      Handle(Type & t):
        _t(&t) {}

      Type * _t;
  };

  public:
    typedef Handle<const Sphere_3> Sphere_handle;
    typedef Handle<const Circle_3> Circle_handle;

    Sphere_intersecter():
      _sphere_tree(), _sphere_storage(),
      _circle_storage(), _stcl(), _ctsl() {} 

    // Make non copyable/assignable
    Sphere_intersecter(const Sphere_intersecter<Kernel> &);
    Sphere_intersecter & operator=(const Sphere_intersecter<Kernel> &);

  private:
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
        { INFER_AUTO(bbox, _handle.get().bbox());
          return Point(bbox.xmin(), bbox.ymin(), bbox.zmin()); }

      private:
        Id _handle;
    };

    // Template typedef for AABB tree of handles
    template <typename Handle>
    struct AABB_handle_tree
    { typedef CGAL::AABB_tree<CGAL::AABB_traits<Kernel,
      AABB_handle_primitive<typename Handle::Type> > > Type; };

    // AABB tree of spheres
    typedef typename AABB_handle_tree<Sphere_handle>::Type Sphere_handle_tree;
    typedef typename Sphere_handle_tree::Primitive Sphere_primitive;

    // Actual list of spheres (used only for storage). Note that
    // we cannot use a vector since the address should remain
    // the same after the first insertion. The linked list defined
    // by the LINKED_LIST macro is sure to support constant-time
    // insertion and lookup at the *front* of the list.
    // This list is optimized for memory efficiency when compiling,
    // with the C++0x version by using a singly linked list,
    // which is the main usage of this storage system.
    typedef LINKED_LIST<Sphere_3> Sphere_storage;
    // ...same for circles
    typedef LINKED_LIST<Circle_3> Circle_storage;

    // Templated typedef for mapping from handle to something (major
    // refactoring for links). Default comparaison is done by handle's
    // pointer object's address.
    template <typename Handle, typename Mapped>
    struct Handle_map
    { typedef std::map<Handle, Mapped> Type; };

    // Link between a sphere and the intersection circles on it
    typedef typename Handle_map<Sphere_handle,
            std::vector<Circle_handle> >::Type Spheres_to_circle_link;
    typedef typename Spheres_to_circle_link::mapped_type Circle_link;

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
    { for (; begin != end; begin++)
      { add_sphere(*begin); } }

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
      PROFILE_SCOPE_WITH_NAME("new_sphere");

      // Store a copy of the inserted sphere and insert
      // a handle of this inserted sphere in the Tree
      _sphere_storage.push_front(sphere_to_insert);
      const Sphere_3 & s1 = _sphere_storage.front();
      Sphere_handle sh1(s1);
      { PROFILE_SCOPE_WITH_NAME("new_sphere::tree_insert");
        _sphere_tree.insert(Sphere_primitive(s1)); }
      CGAL_assertion(_sphere_tree.empty() == false);

      // No need to test for intersections when there is only one element
      if (_sphere_tree.size() > 1)
      {
        // Container receiving intersected spheres
        std::vector<Sphere_handle> intersected;

        // Find intersected balls
        { PROFILE_SCOPE_WITH_NAME("new_sphere_all_intersected_primitives");
          _sphere_tree.all_intersected_primitives(sh1.get(),
              std::inserter(intersected, intersected.begin())); }

        // Handle intersections
        for (INFER_AUTO(it, intersected.begin());
            it != intersected.end(); it++)
        { handle_sphere_intersection(sh1, *it); }
      }
      return sh1;
    }

    void handle_sphere_intersection(const Sphere_handle & sh1,
        const Sphere_handle & sh2)
    {
      PROFILE_SCOPE_WITH_NAME("handle_sphere_intersection");

      // *More* syntaxic sugar
      typename Sphere_handle::Type & s1 = sh1.get();
      typename Sphere_handle::Type & s2 = sh2.get();

      // Skip inserted sphere (comparing by address is *much* faster,
      // and valid since we only store the sphere once, and pass around
      // handles holding a const reference to this sphere)
      if (&s1 == &s2)
      { return; }

      // Insertion of two equal spheres is forbidden here
      CGAL_assertion(s1 != s2);

      // Try intersection
      Object_3 obj = Intersect_3()(s1, s2);

      // No intersection -> end
      if (obj.is_empty())
      { return; }

      // Intersection along a circle
      Circle_3 it_circle;
      if (Assign_3()(it_circle, obj))
      { new_sphere_intersection(sh1, sh2, it_circle);
        return; }

      // Intersection along a point
      Point_3 it_point;
      if (Assign_3()(it_point, obj))
      { new_sphere_intersection(sh1, sh2, Circle_3(it_point, 0, 
              Line_3(s1.center(), s2.center()).perpendicular_plane(it_point)));
        return; }
    }

    // Preconditions:
    //  - this particular intersection hasn't been
    //    inserted until now (checked only for sphere -> circle link)
    void new_sphere_intersection(const Sphere_handle & sh1,
        const Sphere_handle & sh2, const Circle_3 & c)
    {
      PROFILE_SCOPE_WITH_NAME("new_sphere_intersection");

      // Store the circle of intersection
      _circle_storage.push_front(c);
      Circle_handle ch(_circle_storage.front());

      // Setup the links
      _ctsl[ch] = Sphere_handle_pair(sh1, sh2);

      // Compute the intersections between circles on each sphere
      new_circle_on_sphere(sh1, ch);
      new_circle_on_sphere(sh2, ch);
    }

    void new_circle_on_sphere(const Sphere_handle & sh,
        const Circle_handle & ch)
    {
      PROFILE_SCOPE_WITH_NAME("new_circle_on_sphere");

      // Setup the new circle
      Circle_link & sphere_circles = _stcl[sh];
      sphere_circles.insert(sphere_circles.begin(), ch);

      // Handle intersections
      for (INFER_AUTO(it, sphere_circles.begin());
          it != sphere_circles.end(); it++)
      { handle_circle_intersection(ch, *it); }
    }

    void handle_circle_intersection(const Circle_handle & ch1,
        const Circle_handle & ch2)
    {
      PROFILE_SCOPE_WITH_NAME("handle_circle_intersection");

      // *More* syntaxic sugar
      typename Circle_handle::Type & c1 = ch1.get();
      typename Circle_handle::Type & c2 = ch2.get();

      // Ignore self intersecting
      if (&c1 == &c2) { return; }

      // Do intersections
      std::vector<Object_3> intersected;
      Intersect_3()(c1, c2, std::inserter(intersected, intersected.begin()));

      // Handle intersections
      if (intersected.empty() == false)
      { std::cout << "Intersection between circles " << &c1
          << " and " << &c2 << std::endl; }
      for (INFER_AUTO(it, intersected.begin());
          it != intersected.end(); it++)
      {
        if (it->is_empty())
        { continue; }

        std::pair<Circular_arc_point_3, unsigned int> cap;
        if (Assign_3()(cap, *it))
        { std::cout << "- point [" << cap.first << "],"
            << " of multiplicity " << cap.second << std::endl;
          continue; }

        Circle_3 c;
        if (Assign_3()(c, *it))
        { std::cout << "- circle " << c << std::endl;
          continue; }

        std::cout << "Unhandled intersection" << std::endl;
      }
    }

    // Sphere bundle
    Sphere_handle_tree _sphere_tree;
    Sphere_storage _sphere_storage;

    // Circle bundle
    Circle_storage _circle_storage;

    // Spheres <-> Circles
    Spheres_to_circle_link _stcl;
    Circle_to_spheres_link _ctsl;
};

#endif // SPHERE_INTERSECTER_H // vim: sw=2 et ts=2 sts=2
