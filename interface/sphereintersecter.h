#ifndef SPHEREINTERSECTER_H
#define SPHEREINTERSECTER_H

#include <CGAL/Exact_spherical_kernel_3.h>
#include <Sphere_intersecter.h>

typedef CGAL::Exact_spherical_kernel_3 SphereIntersecterKernel;
typedef Sphere_intersecter<SphereIntersecterKernel> SphereIntersecter;
typedef typename SphereIntersecterKernel::Point_3 Point_3;
typedef typename SphereIntersecterKernel::Circle_3 Circle_3;
typedef typename SphereIntersecterKernel::Sphere_3 Sphere_3;

#endif // SPHEREINTERSECTER_H
