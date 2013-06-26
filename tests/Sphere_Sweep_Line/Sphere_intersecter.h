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
#else
#  include <list>
#  define LINKED_LIST std::list
#  define INFER_AUTO BOOST_AUTO
#endif // __GXX_EXPERIMENTAL_CXX0X__ //

#include <CGAL/assertions.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>

#include <Handle.h>

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
    // Precondition: this particular sphere hasn't been inserted yet
    Sphere_handle add_sphere(const Sphere_3 & sphere_to_insert)
    {
      // Store a copy of the inserted sphere
      _sphere_storage.push_front(sphere_to_insert);
      const Sphere_3 & s1 = _sphere_storage.front();
      Sphere_handle sh1(s1);

      // No need to test for intersections when there is only one element
      if (_sphere_tree.size() > 1)
      {
        // Container receiving intersected spheres
        std::vector<Sphere_handle> it_spheres;

        // Find intersected balls
        _sphere_tree.all_intersected_primitives(*sh1,
            std::inserter(it_spheres, it_spheres.begin()));

        // Handle intersections
        for (INFER_AUTO(it, it_spheres.begin()); it != it_spheres.end(); it++)
        {
          // Syntaxic sugar
          Sphere_handle sh2(*it);
          const Sphere_3 & s2 = *sh2;

          // Insertion of two equal spheres is forbidden here
          CGAL_assertion(sh1 != sh2 && s1 != s2);

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
      
      // Insert a handle of the sphere in the tree
      _sphere_tree.insert(Sphere_primitive(s1));
      return sh1;
    }

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

    Sphere_handle find_sphere(const Sphere_3 & s) const
    {
      std::vector<Sphere_handle> it_spheres;
      _sphere_tree.all_intersected_primitives(s,
          std::inserter(it_spheres, it_spheres.begin()));
      INFER_AUTO(it, std::find(it_spheres.begin(),
            it_spheres.end(), s));
      return (it != it_spheres.end()) ? *it : Sphere_handle();
    }

    template <typename OutputIterator>
    OutputIterator circles_on_sphere(const Sphere_3 & s,
        OutputIterator out_it) const
    {
      Sphere_handle sh = find_sphere(s);
      if (sh.is_null() == false)
      { return circles_on_sphere(sh, out_it); }
      return out_it;
    }

    template <typename OutputIterator>
    OutputIterator circles_on_sphere(const Sphere_handle & sh,
        OutputIterator out_it) const
    {
      INFER_AUTO(it, _stcl.find(sh));
      if (it != _stcl.end())
      { std::copy(it->second.begin(), it->second.end(), out_it); }
      return out_it;
    }

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
