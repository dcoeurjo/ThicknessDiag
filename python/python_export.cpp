#include <boost/python.hpp>
#include <CGAL/Exact_spherical_kernel_3.h>
#include <Sphere_intersecter.h>

namespace impl {
    // Spherical kernel
    typedef CGAL::Exact_spherical_kernel_3 SK;

    // Kernel objects
    typedef typename SK::Point_3 Point_3;
    typedef typename SK::Sphere_3 Sphere_3;

    // Sphere_intersecter
    typedef Sphere_intersecter<SK> SI;
}

// Explicit template instanciations
template class Sphere_intersecter<impl::SK>;

// Wrapper for impl::Point_3
class Point
{
    public:
        Point(double x, double y, double z):
            _point(x, y, z),
            _x(x), _y(y), _z(z) {}

        Point(const impl::Point_3 & p):
            _point(p)
        {
            using CGAL::to_double;
            _x = to_double(p.x());
            _y = to_double(p.y());
            _z = to_double(p.z());
        }

        double x() const
        { return _x; }
        double y() const
        { return _y; }
        double z() const
        { return _z; }

        // Convert to impl
        const impl::Point_3 & to_impl() const
        { return _point; }

    private:
        impl::Point_3 _point;
        double _x, _y, _z;
};

// Wrapper for impl::Sphere_3
class Sphere
{
    public:
        Sphere(const Point & p, double radius):
            _sphere(p.to_impl(), radius*radius),
            _center(p), _radius(radius) {}

        Sphere(const impl::Sphere_3 & s):
            _sphere(s), _center(s.center())
        { _radius = std::sqrt(CGAL::to_double(s.squared_radius())); }

        // Getters
        const Point & center() const
        { return _center; }
        double radius() const
        { return _radius; }

        // Convert to impl
        const impl::Sphere_3 & to_impl() const
        { return _sphere; }

    private:
        impl::Sphere_3 _sphere;
        Point _center;
        double _radius;
};

// Wrapper for impl::SI
class SphereIntersecter
{
    public:
        bool add_sphere(const Sphere & s)
        { return _si.add_sphere(s.to_impl()).is_null() == false; }

    private:
        impl::SI _si;
};

BOOST_PYTHON_MODULE(thickness_diag)
{
    using namespace boost::python;

    class_<Point>("Point", init<double, double, double>())
        .add_property("x", &Point::x)
        .add_property("y", &Point::y)
        .add_property("z", &Point::z)
    ;

    class_<Sphere>("Sphere", init<Point, double>())
        .def("center", &Sphere::center,
                return_value_policy<copy_const_reference>())
        .def("radius", &Sphere::radius)
    ;

    class_<SphereIntersecter, boost::noncopyable>("SphereIntersecter")
        .def("add_sphere", &SphereIntersecter::add_sphere)
    ;
}
