#include <boost/python.hpp>

#include <Sphere_intersecter.h>
#include <Event_queue_builder.h>

#include <CGAL/Exact_spherical_kernel_3.h>

// Spherical kernel to use
typedef CGAL::Exact_spherical_kernel_3 SK;

// Sphere intersecter instanciation
template class Sphere_intersecter<SK>;
typedef Sphere_intersecter<SK> SI;

BOOST_PYTHON_MODULE(thickness_diagram)
{
    using namespace boost::python;

    class_<SI>("SphereIntersecter", no_init)
        .def("add_sphere", &SI::add_sphere)
    ;
}
