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
#  include <stdexcept>
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

    private:
      Handle(Type & t):
        _t(&t) {}

      Type * _t;
  };

  public:
    typedef Handle<const Sphere_3> Sphere_handle;
    typedef Handle<const Circle_3> Circle_handle;

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

    // AABB tree of circles
    typedef typename AABB_handle_tree<Circle_handle>::Type Circle_handle_tree;
    typedef typename Circle_handle_tree::Primitive Circle_primitive;

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
    // ...and for circle trees
    typedef SHARED_PTR<Circle_handle_tree> Circle_handle_tree_ptr;
    typedef LINKED_LIST<Circle_handle_tree_ptr> Circle_handle_tree_storage;

    // Compare functor for handles
    template <typename Handle>
    struct Comp_handle_by_ptr
    { bool operator()(const Handle & x,
          const Handle & y) const
      { return x.get_pointer() < y.get_pointer(); } };

    // Templated typedef for mapping from handle to something (major
    // refactoring for links). Default comparaison is done by handle's
    // pointer object's address.
    template <typename Handle, typename Mapped,
       typename Comp = Comp_handle_by_ptr<Handle> >
    struct Handle_map
    { typedef std::map<Handle, Mapped, Comp> Type; };

    // Link between a sphere and the intersection circles on it
    typedef Handle<Circle_handle_tree> Circle_handle_tree_handle;
    typedef typename Handle_map<Sphere_handle,
            Circle_handle_tree_handle>::Type Spheres_to_circle_link;

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
      PROFILE_SCOPE_WITH_NAME("Sphere_intersecter::new_sphere");

      // Store a copy of the inserted sphere and insert
      // a handle of this inserted sphere in the Tree
      _sphere_storage.push_front(sphere_to_insert);
      const Sphere_3 & s1 = _sphere_storage.front();
      _sphere_tree.insert(Sphere_primitive(s1));
      Sphere_handle sh1(s1);

      // No need to test for intersections when there is only one element
      if (_sphere_tree.size() > 1)
      {
        // Container receiving intersected spheres
        std::vector<Sphere_handle> intersected;

        // Find intersected balls
        _sphere_tree.all_intersected_primitives(sh1.get().bbox(),
            std::inserter(intersected, intersected.begin()));

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

      if (obj.is_empty())
      { return; }

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
      _circle_storage.push_front(c);
      const Circle_3 & ch(_circle_storage.front());

      // Setup the links
      _ctsl[ch] = Sphere_handle_pair(sh1, sh2);

      // Compute the intersections between circles on each sphere
      new_circle_on_sphere(sh1, ch);
      new_circle_on_sphere(sh2, ch);
    }

    void new_circle_on_sphere(const Sphere_handle & sh,
        const Circle_handle & ch)
    {
      PROFILE_SCOPE_WITH_NAME("Sphere_intersecter::new_circle_on_sphere");

      // Setup the circle tree if is hasn't yet been
      INFER_AUTO(it, _stcl.find(sh));
      if (it == _stcl.end())
      {
        Circle_handle_tree_handle chth = make_circle_handle_tree_handle();
        it = _stcl.insert(std::make_pair(sh, chth)).first;
      }

      // Insert the new circle in the appropriate tree
      Circle_handle_tree & ch_tree = it->second.get();
      ch_tree.insert(Circle_primitive(ch));

      // No need to test for intersections when there is only one element
      if (ch_tree.size() > 1)
      {
        // Container receiving intersected circles
        std::vector<Circle_handle> intersected;

        // Find intersected balls
        ch_tree.all_intersected_primitives(ch.get().bbox(),
            std::inserter(intersected, intersected.begin()));

        // Handle intersections
        for (INFER_AUTO(it, intersected.begin());
            it != intersected.end(); it++)
        { handle_circle_intersection(ch, *it); }
      }
    }

    Circle_handle_tree_handle make_circle_handle_tree_handle()
    {
      // Allocate and store the new tree
      Circle_handle_tree_ptr cht_ptr(new Circle_handle_tree());
      _circle_handle_tree_storage.push_front(cht_ptr);

      // Return a handle to the newly allocated memory
      return Circle_handle_tree_handle(*cht_ptr);
    }

    void handle_circle_intersection(const Circle_handle & ch1,
        const Circle_handle & ch2)
    {
      //// Ignore self intersecting
      //if (c1 == c2) { return; }

      //// Do intersections
      //std::vector<Object> intersected;
      //intersection(c1, c2, std::back_inserter(intersects_c12));

      //// Handle intersections
      //if (intersects_c12.empty() == false)
      //{
      //  std::cout << "Intersection between circles " << c1
      //    << " and " << c2 << std::endl;
      //}
      //for (std::vector<Object>::const_iterator it = intersects_c12.begin();
      //    it != intersects_c12.end(); it++)
      //{
      //  // Intersection object
      //  const Object & obj = *it;

      //  // Handle intersections
      //  if (const std::pair<Circular_arc_point_3,
      //      unsigned int> * p = object_cast<std::pair<Circular_arc_point_3,
      //      unsigned int> >(&obj))
      //  {
      //    std::cout << "- point [" << p->first << "],"
      //      << " of multiplicity " << p->second << std::endl;
      //  }
      //  else if (const Circle_3 * c = object_cast<Circle_3>(&obj))
      //  {
      //    std::cout << "- circle " << *c << std::endl;
      //  }
      //  else
      //  {
      //    std::cout << "Unhandled intersection" << std::endl;
      //  }
      //}
    }

    // Sphere bundle
    Sphere_handle_tree _sphere_tree;
    Sphere_storage _sphere_storage;

    // Circle bundle
    Circle_storage _circle_storage;
    Circle_handle_tree_storage _circle_handle_tree_storage;

    // Spheres <-> Circles
    Spheres_to_circle_link _stcl;
    Circle_to_spheres_link _ctsl;
};

#endif // SPHERE_INTERSECTER_H // vim: sw=2 et ts=2 sts=2
