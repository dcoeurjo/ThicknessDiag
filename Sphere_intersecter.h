#ifndef SPHERE_INTERSECTER_H
#define SPHERE_INTERSECTER_H

#include <map>
#include <vector>
#include <memory>
#include <iterator>

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#  define INFER_AUTO(x, y) auto x(y)
#else
#  define INFER_AUTO BOOST_AUTO
#endif // __GXX_EXPERIMENTAL_CXX0X__ //

#include <CGAL/assertions.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>

#include <Handle.h>

template <typename SK>
class Sphere_intersecter_insert_iterator;

template <typename SK>
class Sphere_intersecter
{
  // Geometric objects bundle
  typedef typename SK::Point_3 Point_3;
  typedef typename SK::Sphere_3 Sphere_3;
  typedef typename SK::Circle_3 Circle_3;
  typedef typename SK::Line_3 Line_3;
  typedef typename SK::Circular_arc_3 Circular_arc_3;
  typedef typename SK::Circular_arc_point_3 Circular_arc_point_3;

  // Intersection bundle
  typedef typename SK::Object_3 Object_3;
  typedef typename SK::Assign_3 Assign_3;
  typedef typename SK::Do_intersect_3 Do_intersect_3;
  typedef typename SK::Intersect_3 Intersect_3;

  // Friend access
  friend class Sphere_iterator_range;

  public:
    typedef Handle<const Circle_3> Circle_handle;
    typedef Handle<const Sphere_3> Sphere_handle;
    typedef std::pair<Sphere_handle, Sphere_handle> Sphere_handle_pair;

    Sphere_intersecter():
      _sphere_tree(), _sphere_storage(),
      _circle_storage(), _stcl(), _ctsl() {}

    // Make non copyable/assignable
    Sphere_intersecter(const Sphere_intersecter<SK> &);
    Sphere_intersecter & operator=(const Sphere_intersecter<SK> &);

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
    { typedef CGAL::AABB_tree<CGAL::AABB_traits<SK,
      AABB_handle_primitive<typename Handle::Type> > > Type; };

    // AABB tree of spheres
    typedef typename AABB_handle_tree<Sphere_handle>::Type Sphere_handle_tree;
    typedef typename Sphere_handle_tree::Primitive Sphere_primitive;

    // Actual list of spheres (used only for storage). Note that
    // we cannot use a vector since the address should remain
    // the same after the first insertion.
    typedef std::list<Sphere_3> Sphere_storage;
    // ...same for circles
    typedef std::list<Circle_3> Circle_storage;

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
    typedef typename Handle_map<Circle_handle,
            Sphere_handle_pair>::Type Circle_to_spheres_link;

  public:
    // Add a new sphere, returning a sphere handle (null if not added)
    Sphere_handle add_sphere(const Sphere_3 &);

    typedef Sphere_intersecter_insert_iterator<SK>
      Sphere_insert_iterator;

    Sphere_insert_iterator insert_iterator()
    { return Sphere_insert_iterator(*this); }

    typedef Handle_iterator<typename Sphere_storage::const_iterator>
      Sphere_iterator;

    class Sphere_iterator_range
    {
      typedef Sphere_intersecter<SK> SI;

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

    Sphere_handle find_sphere(const Sphere_3 & s) const;

    Circle_handle find_circle_in_sphere(const Sphere_handle &,
        const Circle_3 &) const;

    Circle_handle find_circle_in_sphere(const Sphere_3 &,
        const Circle_3 &) const;

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

    Sphere_handle_pair originating_spheres(const Circle_handle &) const;

    // Removes a sphere
    bool remove_sphere(const Sphere_handle &);

    // Bounding box
    typedef typename Sphere_handle_tree::Bounding_box Bounding_box;

    // Entire spheres' bounding box
    Bounding_box bbox() const;

  private:
    void remove_sphere_links(const Sphere_handle &);

    // Sphere bundle
    Sphere_handle_tree _sphere_tree;
    Sphere_storage _sphere_storage;

    // Circle bundle
    Circle_storage _circle_storage;

    // Spheres <-> Circles
    Spheres_to_circle_link _stcl;
    Circle_to_spheres_link _ctsl;
};

template <typename SK>
class Sphere_intersecter_insert_iterator:
  public std::iterator<std::output_iterator_tag,
  void, void, void, void>
{
  typedef Sphere_intersecter_insert_iterator<SK> Self;
  typedef Sphere_intersecter<SK> SI;
  typedef typename SK::Sphere_3 Sphere_3;

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
