#ifndef SPHEREINTERSECTER_H
#define SPHEREINTERSECTER_H

#include <Sphere_intersecter.h>
#include "kernel.h"

// Shortcuts for sphere intersecter
typedef Sphere_intersecter<Kernel> SphereIntersecter;
typedef SphereIntersecter::Sphere_handle SphereHandle;
typedef SphereIntersecter::Sphere_iterator SphereIterator;
typedef SphereIntersecter::Circle_handle CircleHandle;
typedef SphereIntersecter::Bounding_box BoundingBox;

#endif // SPHEREINTERSECTER_H
