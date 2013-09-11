#ifndef SPHERICAL_UTILS_H
#define SPHERICAL_UTILS_H

#include <functional>

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

#endif // SPHERICAL_UTILS_H // vim: ft=cpp et sw=2 sts=2
