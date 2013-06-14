#ifndef CIRCLE_PROXY_H
#define CIRCLE_PROXY_H

#include <set>
#include <vector>
#include <iterator>
#include <functional>
#include <algorithm>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

#include <Spherical_utils.h>

template <typename Kernel>
class Circle_proxy;

template <typename Kernel>
class Circular_arc_handle;

template <typename Kernel>
class Circle_handle
{
  public:
    // Split the circle, requires concept that point(s)
    // given are on the circle
    //
    // ...using one point
    Circular_arc_handle<Kernel> split(typename Kernel::Point_3 const & p)
    { return _cp.split_circle(*this, p); }
    // ...using two points
    Circular_arc_handle<Kernel> split(typename Kernel::Point_3 const & source,
        typename Kernel::Point_3 const & target)
    { return _cp.split_circle(*this, source, target); }

    // Get the underlying circle
    typename Kernel::Circle_3 const & get() const
    { return _circle; }

  private:
    // Bidirectional exclusive access
    friend class Circle_proxy<Kernel>;

    // Can only be constructed from proxy
    Circle_handle(Circle_proxy<Kernel> & cp,
        typename Kernel::Circle_3 const & circle):
      _cp(cp), _circle(circle) {}

    // Reference to parent
    Circle_proxy<Kernel> & _cp;
    // Underlying circle
    typename Kernel::Circle_3 const & _circle;
};

template <typename Kernel>
class Circular_arc_handle
{
  public:
    // Get the underlying arc
    typename Kernel::Circular_arc_3 const & get() const
    { return _arc; }

    // Get a circle handle linked to this arc
    Circle_handle<Kernel> circle()
    { return _cp.make_circle_handle(_arc.supporting_circle()); }

  private:
    // Bidirectional exclusive access
    friend class Circle_proxy<Kernel>;

    // Can only be constructed from proxy
    Circular_arc_handle(Circle_proxy<Kernel> & cp,
        typename Kernel::Circular_arc_3 const & arc):
      _cp(cp), _arc(arc) {}

    // Reference to parent
    Circle_proxy<Kernel> & _cp;
    // Underlying arc
    typename Kernel::Circular_arc_3 const & _arc;
};

template <typename Kernel>
class Circle_proxy_adder;

template <typename Kernel>
class Circle_proxy_handle_range;

template <typename Kernel>
class Circle_proxy
{
  public:
    // Access from outside
    typedef Circle_proxy_adder<Kernel>
      Circle_adder;
    typedef Circle_proxy_handle_range<Kernel>
      Circle_handle_range;

    // Add a circle to the proxy, computing intersections
    //  and inserting these computed intersection arcs
    Circle_handle<Kernel> add_circle(typename Kernel::Circle_3 const & c)
    {
#ifndef NDEBUG
      std::pair<Circle_iterator, Circle_iterator> it_range;
      it_range = _circle_list.equal_range(c);
      if (std::find(it_range.first, it_range.second, c) != it_range.second)
      {
        std::ostringstream oss;
        oss << "Concept violation, cannot insert twice the same"
          << " circle in the circle proxy" << std::endl
          << "Found range:" << std::endl;
        for (Circle_iterator it = it_range.first;
            it != it_range.second; it++)
        {
          oss << "  " << *it << std::endl;
        }
        oss << std::endl
          << "Equal to: " << c << std::endl;
        throw std::logic_error(oss.str());
      }
#endif // NDEBUG //
      return Circle_handle<Kernel>(*this, *_circle_list.insert(c));
    }

    // Iterate over all circles with handles
    Circle_handle_range circles() const
    { return Circle_handle_range(*this); }

    Circle_adder adder()
    { return Circle_adder(*this); }

  private:
    // Bidirectional exclusive access
    friend class Circle_handle<Kernel>;
    friend class Circular_arc_handle<Kernel>;

    // Sole access to list and iterators
    friend class Circle_proxy_handle_range<Kernel>;

    // Set of circles
    typedef std::multiset<typename Kernel::Circle_3,
            Comp_by_squared_radii<typename Kernel::Circle_3> > Circle_list;

    // Circle iterator
    typedef typename Circle_list::const_iterator
      Circle_iterator;

    // List of arcs
    typedef std::vector<typename Kernel::Circular_arc_3> Arc_list;
    Arc_list _arcs;

    Circle_handle<Kernel> make_circle_handle(typename
        Kernel::Circle_3 const & circle) const
    {
      Circle_iterator it = _circle_list.find(circle);
#ifndef NDEBUG
      if (it == _circle_list.end())
      {
        std::ostringstream oss;
        oss << "Cannot make circle handle from circle which isn't"
          << " present in the circle proxy";
        throw std::logic_error(oss.str());
      }
#endif // NDEBUG //
      return Circle_handle<Kernel>(*it);
    }

    Circular_arc_handle<Kernel> split_circle(Circle_handle<Kernel> & ch,
        typename Kernel::Point_3 const & p)
    {
      typedef typename Kernel::Circular_arc_3 Arc;
      _arcs.push_back(Arc(ch.get(), p));
      Arc const & inserted_arc = _arcs.back();
      return Circular_arc_handle<Kernel>(*this, inserted_arc);
    }

    std::pair<Circular_arc_handle<Kernel>,
      Circular_arc_handle<Kernel> > split_circle(Circle_handle<Kernel> & ch,
        typename Kernel::Point_3 const & source,
        typename Kernel::Point_3 const & target)
    {
      typedef typename Kernel::Circular_arc_3 Arc;
      _arcs.push_back(Arc(ch.get(), source, target));
      Arc const & first_arc = _arcs.back();
      _arcs.push_back(Arc(ch.get(), target, source));
      Arc const & second_arc = _arcs.back();
      return std::make_pair(Circular_arc_handle<Kernel>(*this, first_arc),
          Circular_arc_handle<Kernel>(*this, second_arc));
    }

    // Actual set of circles
    Circle_list _circle_list;
};

template <typename Kernel>
class Circle_proxy_adder
{
  public:
    Circle_proxy_adder(Circle_proxy<Kernel> & cp):
      _cp(cp) {}

    Circle_handle<Kernel>
      operator()(typename Kernel::Circle_3 const & c)
    { return _cp.add_circle(c); }

  private:
    Circle_proxy<Kernel> & _cp;
};

template <typename Kernel>
class Circle_proxy_handle_range
{
  public:
    Circle_proxy_handle_range(Circle_proxy<Kernel> & cp):
      _cp(cp) {}

    class iterator:
      public std::iterator<std::bidirectional_iterator_tag,
      Circle_handle<Kernel> >
    {
      public:
        iterator(typename Circle_proxy<Kernel>::Circle_iterator const & it):
          _it(it) {}

        iterator & operator++()
        { ++_it; return *this; }

        iterator operator++(int)
        { iterator tmp(*this); operator++(); return tmp; }

        iterator & operator--()
        { --_it; return *this; }

        iterator operator--(int)
        { iterator tmp(*this); operator--(); return tmp; }

        bool operator==(const iterator & it)
        { return _it == it._it; }

        bool operator!=(const iterator& it)
        { return !(*this == it); }

        Circle_handle<Kernel> operator*()
        { return *_it; }

      private:
        typename Circle_proxy<Kernel>::Circle_iterator _it;
    };

    iterator begin()
    { return iterator(_cp._circle_list.begin()); }

    iterator end()
    { return iterator(_cp._circle_list.end()); }

  private:
    Circle_proxy<Kernel> & _cp;
};

#endif // CIRCLE_PROXY_H // vim: sw=2 et ts=2 sts=2
