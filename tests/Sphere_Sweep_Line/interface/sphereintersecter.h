#ifndef SPHEREINTERSECTER_H
#define SPHEREINTERSECTER_H

#include <CGAL/Exact_spherical_kernel_3.h>
#include <Sphere_intersecter.h>

typedef CGAL::Exact_spherical_kernel_3 SphereIntersecterKernel;
typedef Sphere_intersecter<SphereIntersecterKernel> SphereIntersecter;

#endif // SPHEREINTERSECTER_H
