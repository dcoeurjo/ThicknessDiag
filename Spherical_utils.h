#ifndef CIRCLE_UTILS_H
#define CIRCLE_UTILS_H

#include <functional>

#include <CGAL/Kernel/global_functions_3.h>
#include <CGAL/Spherical_kernel_3.h>

template <typename Squared_radius_holder>
struct Comp_by_squared_radii:
  public std::unary_function<bool, Squared_radius_holder>
{
  bool operator()(const Squared_radius_holder & left,
      const Squared_radius_holder & right) const
  {
    return left.squared_radius() < right.squared_radius();
  }
};

#endif // CIRCLE_UTILS_H // vim: sw=2 et ts=2 sts=2
