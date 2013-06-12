#ifndef VERTICAL_ORDERER_H
#define VERTICAL_ORDERER_H

#include <vector>
#include <functional>
#include <algorithm>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

#include <circle_utils.h>
#include <circle_proxy.h>

template <typename Kernel>
class Vertical_orderer
{
  public:
    Vertical_orderer(const Circle_proxy<Kernel> & cp):
      _circle_proxy(cp) {}

    // Concept of initial insertion:
    //  - iterators must follow STL's ForwardIterator concept
    //  - elements must be Kernel::Circular_arc_3 arcs
    //  - arcs must be tagged as either start or end,
    //      and correspond to normal events
    template <typename ForwardIterator>
    void initial_insert(ForwardIterator begin, ForwardIterator end)
    {
#ifndef NDEBUG // Check assertions
      if (_arcs.empty() == false) // Vertical orderer clean
      {
        std::ostringstream oss;
        oss << "Initial insertion can only be done when the vertical"
          << " orderer is in a clean state, via the clear() method";
        throw std::logic_error(oss.str());
      }
      if (begin == end) // Valid input iterators
      {
        std::ostringstream oss;
        oss << "Initial insertion must be done with"
          << " at least one element";
        throw std::logic_error(oss.str());
      }
#endif // NDEBUG //
      ForwardIterator me = std::max_element(begin, end, 
          Comp_arcs_by_radii<Kernel>());
      typename Kernel::Point_3 start_pt(me->source()),
               end_pt(me->target());
      // TODO initial "sort and insert"
    }

    void insert(typename Kernel::Circular_arc_3 const & arc)
    { _arcs.insert(arc); }

    void remove(typename Kernel::Circular_arc_3 const & arc)
    { _arcs.erase(arc); }

    void contains(typename Kernel::Circular_arc_3 const & arc) const
    { return _arcs.find(arc) != _arcs.end(); }

    void clear()
    { _arcs.clear(); }

  private:
    // List of arcs sorted "vertically"
    std::vector<typename Kernel::Circular_arc_3> _arcs;

    // Circle proxy
    const Circle_proxy<Kernel> & _circle_proxy;
};

#endif // VERTICAL_ORDERER_H // vim: sw=2 et ts=2 sts=2
