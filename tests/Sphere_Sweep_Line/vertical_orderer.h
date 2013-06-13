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
    void insert_starting_arcs(ForwardIterator begin, ForwardIterator end)
    {
#ifndef NDEBUG // Check assertions
      if (begin == end) // Valid input iterators
      {
        std::ostringstream oss;
        oss << "Initial insertion must be done with"
          << " at least one element";
        throw std::logic_error(oss.str());
      }
#endif // NDEBUG //

      // Setup compare functor
      ForwardIterator me = std::max_element(begin, end, 
          Comp_arcs_by_radii<Kernel>());
      _comp.base_arc = *me;
      // TODO start point
    }

    void insert(typename Kernel::Circular_arc_3 const & arc)
    { _loc.update(arc); _arcs.insert(arc); }

    void remove(typename Kernel::Circular_arc_3 const & arc)
    { _arcs.erase(arc); }

    void contains(typename Kernel::Circular_arc_3 const & arc) const
    { return _arcs.find(arc) != _arcs.end(); }

    void clear()
    { _arcs.clear(); }

  private:
    // Helper enum providing the three possible values for
    // the location of a point on an arc
    enum Point_location_on_arc {
      Above, Below, On
    };

    // Functor providing the locating operation w.r.t a base point p.
    // This is useful for when "point" is the center of the circle
    // (arc) of largest radius.
    struct Locate_point_on_arc 
    {
      Point_location_on_arc locate(typename
          Kernel::Circular_arc_3 const & arc) const
      {
        // Basic case: point is on the arc
        typename Kernel::Point_3 const & p = base_start_pt;
        if (arc.has_on(p))
        { return On; }

        // Plane passing through the arc's circle
        typename Kernel::Plane_3 plane = arc.supporting_plane();

        // Correct the orientation of the plane s.t the normal
        // makes an acute angle with the z-axis
        typename Kernel::Vector_3 n = plane.orthogonal_vector();
        // TODO

        // Point is above if it is on the plane's positive side
        if (plane.has_on_positive_side(p))
        { return Above; }
        else
        { return Below; }
      }

      // Used to update the compare functor before insertion
      // of a new arc. This arc is the new base if it has a bigger
      // radius than the previous base.
      void update(typename Kernel::Circular_arc_3 const & arc, 
          typename Kernel::Point_3 const & start_pt)
      {
        if (comp_radii(base_arc, arc))
        {
          base_arc = arc;
          base_start_pt = start_pt;
        }
      }

      // Used to compare arcs by radii
      Comp_arcs_by_radii<Kernel> comp_radii;

      // Point to compare with
      typename Kernel::Point_3 base_start_pt;
      // ...arc with biggest radius
      typename Kernel::Circular_arc_3 base_arc;
    };

    // List of arcs sorted "vertically"
    std::vector<typename Kernel::Circular_arc_3> _arcs;

    // Compare functor for sorting arcs
    Locate_point_on_arc _loc;

    // Circle proxy
    const Circle_proxy<Kernel> & _circle_proxy;
};

#endif // VERTICAL_ORDERER_H // vim: sw=2 et ts=2 sts=2
