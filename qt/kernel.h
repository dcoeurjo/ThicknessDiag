#ifndef KERNEL_H
#define KERNEL_H

#include <CGAL/Exact_spherical_kernel_3.h>
typedef CGAL::Exact_spherical_kernel_3 Kernel;

// Geometric objects
typedef typename Kernel::Point_3 Point_3;
typedef typename Kernel::Circle_3 Circle_3;
typedef typename Kernel::Vector_3 Vector_3;
typedef typename Kernel::Sphere_3 Sphere_3;

#endif // KERNEL_H
