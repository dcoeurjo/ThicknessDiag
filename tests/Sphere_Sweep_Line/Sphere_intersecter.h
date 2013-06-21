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

#include <CGAL/assertions.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>

#define DELEGATE_COMPARAISON_OPERATORS(cl, mem) \
  bool operator<(const cl & o) const            \
  { return mem < o.mem; }                       \
  bool operator>(const cl & o) const            \
  { return mem > o.mem; }                       \
  bool operator<=(const cl & o) const           \
  { return mem <= o.mem; }                      \
  bool operator>=(const cl & o) const           \
  { return mem >= o.mem; }                      \
  bool operator==(const cl & o) const           \
  { return mem == o.mem; }                      \
  bool operator!=(const cl & o) const           \
  { return mem != o.mem; }

// Handle class, emulating pointer functionality
// while hiding ownership (a handle doesn't own the
// pointed member object)
template <typename T>
class Handle
{
  public:
    typedef T Type;

    Handle():
      _t(0) {}

    Handle(Type & t):
      _t(&t) {}

    bool is_null() const
    { return _t == 0; }

    Type * const ptr() const
    { return _t; }

    Type & ref() const
    { return *_t; }

    Type & operator*() const
    { return ref(); }

    Type * const operator->() const
    { return ptr(); }

    Type * const operator&() const
    { return ptr(); }

    DELEGATE_COMPARAISON_OPERATORS(Handle<T>, _t)

  private:
    Type * _t;
};

template <typename Iterator>
class Handle_iterator:
  public std::iterator<std::input_iterator_tag,
  Handle<typename Iterator::value_type> >
{
  typedef Iterator It;
  typedef Handle_iterator<It> Self;

  public:
  typedef Handle<typename Iterator::value_type> Handled;

  Handle_iterator(const It & it):
    _it(it) {}

  Self & operator++()
  { ++_it; return *this; }

  Self operator++(int)
  { Self tmp(*this);
    ++(*this); return tmp; }

  bool operator==(const Self & sit) const
  { return _it == sit._it; }

  bool operator!=(const Self & sit) const
  { return !(*this == sit); }

  Handled operator*()
  { return Handled(*_it); }

  private:
  It _it;
};

template <typename Kernel>
class Sphere_intersecter_insert_iterator;

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
  friend class Sphere_iterator_range;

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
        { return *_handle; }

        Id id() const
        { return _handle; }

        Point reference_point() const
        { INFER_AUTO(bbox, _handle->bbox());
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
    // Add a new sphere
    Sphere_handle add_sphere(const Sphere_3 & sphere_to_insert)
    {
      // Store a copy of the inserted sphere and insert
      // a handle of this inserted sphere in the Tree
      _sphere_storage.push_front(sphere_to_insert);
      const Sphere_3 & s1 = _sphere_storage.front();
      Sphere_handle sh1(s1);
      _sphere_tree.insert(Sphere_primitive(s1));
      CGAL_assertion(_sphere_tree.empty() == false);

      // No need to test for intersections when there is only one element
      if (_sphere_tree.size() > 1)
      {
        // Container receiving intersected spheres
        std::vector<Sphere_handle> intersected;

        // Find intersected balls
        _sphere_tree.all_intersected_primitives(*sh1,
            std::inserter(intersected, intersected.begin()));

        // Handle intersections
        for (INFER_AUTO(it, intersected.begin());
            it != intersected.end(); it++)
        {
          // Syntaxic sugar
          Sphere_handle sh2(*it);
          const Sphere_3 & s2 = *sh2;

          // Skip inserted sphere (comparing by address is *much* faster,
          // and valid since we only store the sphere once, and pass around
          // handles holding a const reference to this sphere)
          if (&s1 == &s2)
          { continue; }

          // Insertion of two equal spheres is forbidden here
          CGAL_assertion(s1 != s2);

          // Try intersection
          Object_3 obj = Intersect_3()(s1, s2);

          // No intersection -> end
          if (obj.is_empty())
          { continue; }

          // Different intersections
          Circle_3 it_circle;
          Point_3 it_point;
          if (Assign_3()(it_circle, obj) == false && Assign_3()(it_point, obj))
          { Line_3 it_line(s1.center(), s2.center());
            it_circle = Circle_3(it_point, 0,
                it_line.perpendicular_plane(it_point)); }

            // Store the circle of intersection
            _circle_storage.push_front(it_circle);
            Circle_handle ch(_circle_storage.front());

            // Setup the links
            _ctsl[ch] = Sphere_handle_pair(sh1, sh2);
            Circle_link & sc1 = _stcl[sh1]; sc1.insert(sc1.begin(), ch);
            Circle_link & sc2 = _stcl[sh2]; sc2.insert(sc2.begin(), ch);
        }
      }
      return sh1;
    }

    template <typename InputIterator>
    void add_sphere(InputIterator begin, InputIterator end)
    { for (; begin != end; begin++)
      { add_sphere(*begin); } }

    typedef Sphere_intersecter_insert_iterator<Kernel>
      Sphere_insert_iterator;

    Sphere_insert_iterator insert_iterator()
    { return Sphere_insert_iterator(*this); }

    typedef Handle_iterator<typename Sphere_storage::const_iterator>
      Sphere_iterator;

    class Sphere_iterator_range
    {
      typedef Sphere_intersecter<Kernel> SI;

      public:
        Sphere_iterator_range(const SI & si):
          _si(si) {}

        Sphere_iterator begin() const
        { return Sphere_iterator(_si._sphere_storage.begin()); }

        Sphere_iterator end() const
        { return Sphere_iterator(_si._sphere_storage.end()); }

      private:
        const SI & _si;
    };

    Sphere_iterator_range spheres() const
    { return Sphere_iterator_range(*this); }

  private:
    // Sphere bundle
    Sphere_handle_tree _sphere_tree;
    Sphere_storage _sphere_storage;

    // Circle bundle
    Circle_storage _circle_storage;

    // Spheres <-> Circles
    Spheres_to_circle_link _stcl;
    Circle_to_spheres_link _ctsl;
};

template <typename Kernel>
class Sphere_intersecter_insert_iterator:
  public std::iterator<std::output_iterator_tag,
  void, void, void, void>
{
  typedef Sphere_intersecter_insert_iterator<Kernel> Self;
  typedef Sphere_intersecter<Kernel> SI;
  typedef typename Kernel::Sphere_3 Sphere_3;

  public:
    Sphere_intersecter_insert_iterator(SI & si):
      _si(si) {}

    Self & operator=(const Sphere_3 & s)
    { _si.add_sphere(s); 
    return *this; }

    Self & operator++()
    { return *this; }
    Self & operator++(int)
    { return *this; }
    Self & operator*()
    { return *this; }

  private:
    SI & _si;
};

#endif // SPHERE_INTERSECTER_H // vim: sw=2 et ts=2 sts=2
