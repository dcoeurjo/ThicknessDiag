#include <ctime>
#include <cstddef>

#include <list>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

#include <CGAL/Exact_spherical_kernel_3.h>
#include <CGAL/Random.h>

#include <circle_proxy.h>

typedef CGAL::Exact_spherical_kernel_3 Kernel;

typedef typename Kernel::FT FT;
typedef typename Kernel::Point_3 Point_3;
typedef typename Kernel::Plane_3 Plane_3;
typedef typename Kernel::Vector_3 Vector_3;
typedef typename Kernel::Circle_3 Circle_3;
typedef typename Kernel::Sphere_3 Sphere_3;
typedef typename Kernel::Circular_arc_3 Circular_arc_3;

typedef CGAL::Object Object;

using CGAL::intersection;
using CGAL::object_cast;

typedef CGAL::Random Random;

namespace internal {
    typedef Circle_proxy<Kernel> Circle_proxy;
    typedef Circle_handle<Kernel> Circle_handle;
    typedef Circular_arc_handle<Kernel> Circular_arc_handle;

    class Random_sphere_3
    {
        public:
            Random_sphere_3(FT amp):
                _amp(amp), _rand(),
                _spheres() {}

            Sphere_3 operator()()
            {
                FT x = _amp*_rand.get_double();
                FT y = _amp*_rand.get_double();
                FT z = _amp*_rand.get_double();
                FT r = _amp*_rand.get_double();
                Sphere_3 s(Point_3(x, y, z), r);
                std::list<Sphere_3>::const_iterator it;
                it = std::find(_spheres.begin(), _spheres.end(), s);
                if (it != _spheres.end())
                {
                    return (*this)();
                }
                _spheres.push_back(s);
                return _spheres.back();
            }

        private:
            FT _amp;
            Random _rand;
            std::list<Sphere_3> _spheres;
    };
}

static const double RAND_AMP_DEFAULT = 20;

static std::string usage()
{
    std::ostringstream oss;
    oss << "Usage: circle_proxy NB_SPHERES [RAND_AMP="
        << RAND_AMP_DEFAULT << "]" << std::endl;
    return oss.str();
}

static void do_main(int argc, const char * argv[])
{
    // Parse command line arguments
    if (argc < 2)
    { throw std::runtime_error("Need number of spheres to work with"); }

    // Amount of spheres to intersect
    std::size_t nb_spheres;

    // Convert nb_spheres
    {
        std::istringstream iss(argv[1]);
        if ( (iss >> nb_spheres) == false )
        { throw std::runtime_error("Bad number format (positive integer)"); }
    }

    // Random amplitude for sphere generation
    FT rand_amp = RAND_AMP_DEFAULT;
    if (argc > 2)
    {
        // Too many arguments
        if (argc != 2)
        { throw std::runtime_error("Too many arguments specified"); }

        // Convert rand_amp
        std::istringstream iss(argv[2]);
        if ( (iss >> rand_amp) == false )
        { throw std::runtime_error("Bad number format (real number)"); }
    }

    // Circle proxy to test
    internal::Circle_proxy cp;

    // Spheres to intersect with
    std::vector<Sphere_3> spheres;
    std::generate_n(std::back_inserter(spheres), nb_spheres,
            internal::Random_sphere_3(rand_amp));

    // Print out generated spheres
    std::cout << "Generated spheres :" << std::endl;
    for (std::vector<Sphere_3>::size_type i = 0; i < spheres.size(); i++)
    { std::cout << "[" << i << "] " << spheres[i] << std::endl; }

    // Do intersections
    for (std::vector<Sphere_3>::const_iterator it = spheres.begin();
            it != spheres.end(); it++)
    {
        for (std::vector<Sphere_3>::const_iterator it_2 = spheres.begin();
                it_2 != spheres.end(); it_2++)
        {
            // Ignore self intersecting
            if (it == it_2) { continue; }

            // Try intersection
            Object obj = intersection(*it, *it_2);

            // Only handle circle intersections
            if (const Circle_3 * c1_ptr = object_cast<Circle_3>(&obj))
            {
                const Circle_3 & c1 = *c1_ptr;
                internal::Circle_handle ch1 = cp.add_circle(c1);
                //std::cout << "Comparing " << c1 << " and " << ch1.get() << std::endl;
                CGAL_assertion(c1 == ch1.get());
            }
        }
    }
}

int main(int argc, const char * argv[])
{
    try
    {
        do_main(argc, argv);
        return EXIT_SUCCESS;
    }
    catch (std::runtime_error e)
    {
        std::cerr << e.what() << std::endl
            << std::endl
            << usage() << std::endl;
        return EXIT_FAILURE;
    }
}
