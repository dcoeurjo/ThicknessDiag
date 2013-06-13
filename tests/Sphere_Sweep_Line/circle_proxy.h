#ifndef CIRCLE_PROXY_H
#define CIRCLE_PROXY_H

#include <set>
#include <vector>
#include <iterator>
#include <functional>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

#include <boost/ref.hpp>

template <typename Kernel>
class Circle_proxy;

template <typename Kernel>
class Circle_handle
{
  public:
    // Split the circle
    //
    // Concept: points given are on the circle
    //
    // ...using one point
    typename Kernel::Circular_arc_3
    split(typename Kernel::Point_3 const & p)
    {
      typename Kernel::Circular_arc_3 arc(_circle, p);
      _cp.circle_handle_split(*this, arc);
      return arc;
    }
    // ...using two points
    typename Kernel::Circular_arc_3
    split(typename Kernel::Point_3 const & source,
        typename Kernel::Point_3 const & target)
    {
      typename Kernel::Circular_arc_3 arc(_circle, source, target);
      _cp.circle_handle_split(*this, arc);
      return arc;
    }

    typename Kernel::Circle_3 const & circle() const
    { return _circle; }

  private:
    // Bidirectional exclusive access
    friend class Circle_proxy<Kernel>;

    Circle_handle(Circle_proxy<Kernel> & cp,
        typename Kernel::Circle_3 const & circle):
      _cp(cp), _circle(circle) {}

    typedef std::vector<boost::reference_wrapper<
      const typename Kernel::Circular_arc_3> > Arc_list;

    Circle_proxy<Kernel> & _cp;
    typename Kernel::Circle_3 const & _circle;
    Arc_list _arcs;
};

template <typename Kernel>
class Circle_proxy
{
  public:
    // Add a circle to the proxy, computing intersections
    //  and inserting these computed intersection arcs
    Circle_handle<Kernel> add_circle(typename Kernel::Circle_3 const & c)
    {
      std::pair<Circle_iterator,
        bool> it_pair = _circle_list.insert(c);
#ifndef NDEBUG
      if (it_pair.second == false)
      {
        std::ostringstream oss;
        oss << "Concept violation, cannot insert twice the same"
          << " circle in the circle proxy";
        throw std::logic_error(oss.str());
      }
#endif // NDEBUG //
      return make_circle_handle(it_pair.first);
    }

  private:
    // Bidirectional exclusive access
    friend class Circle_handle<Kernel>;

    // Set of circles
    typedef std::set<typename Kernel::Circle_3>
      Circle_list;

    // Circle iterators
    // ...usual
    typedef typename Circle_list::iterator
      Circle_iterator;
    // ...const
    typedef typename Circle_list::const_iterator
      Circle_const_iterator;

    void circle_handle_split(Circle_handle<Kernel> & ch,
        typename Kernel::Circular_arc_3 const & arc)
    {
      // TODO
    }

    // Actual set of circles
    Circle_list _circle_list;
};

#endif // CIRCLE_PROXY_H // vim: sw=2 et ts=2 sts=2
