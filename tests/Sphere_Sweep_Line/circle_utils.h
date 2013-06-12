#ifndef CIRCLE_UTILS_H
#define CIRCLE_UTILS_H

#include <functional>

template <typename Kernel>
struct Comp_arcs_by_radii
{
    bool operator()(typename Kernel::Circular_arc_3 const & left,
            typename Kernel::Circular_arc_3 const & right) const
    {
        return left.squared_radius() < right.squared_radius();
    }
};

#endif
