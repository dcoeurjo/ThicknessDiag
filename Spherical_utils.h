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

template <typename Kernel>
struct Comp_theta_z_3
{
  typedef typename Kernel::Sphere_3 Sphere_3;
  typedef typename Kernel::Circular_arc_point_3 Circular_arc_point_3;

  bool operator()(const Circular_arc_point_3 & left,
      const Circular_arc_point_3 & right, const Sphere_3 & sphere) const
  { return CGAL::compare_theta_z(left, right, sphere) == CGAL::SMALLER; }
};

#endif // CIRCLE_UTILS_H // vim: sw=2 et ts=2 sts=2
