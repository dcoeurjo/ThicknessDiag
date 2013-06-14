#include <ctime>
#include <cstddef>

#include <list>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

#include <CGAL/Exact_spherical_kernel_3.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Filtered_kernel.h>
#include <CGAL/Algebraic_kernel_for_spheres_2_3.h>
#include <CGAL/Spherical_kernel_3.h>
#include <CGAL/Random.h>

#include <circle_proxy.h>

// Faster kernel
typedef CGAL::Simple_cartesian<double> CK;
typedef CGAL::Algebraic_kernel_for_spheres_2_3<CK::RT> AKS;
typedef CGAL::Spherical_kernel_3<CK, AKS> SK;

// Exact kernel
typedef CGAL::Exact_spherical_kernel_3 ESK;

// Definition of actually used kernel
typedef ESK Kernel;

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

    class Add_and_check_circle
    {
        public:
            Add_and_check_circle(Circle_proxy & cp):
                _ca(cp.adder()) {}

            void operator()(const Circle_3 & c)
            {
                Circle_handle ch = _ca(c);
                if (c != ch.get())
                {
                    std::ostringstream oss;
                    oss << "Invalid insertion of circle " << c
                        << ", is different from inserted circle "
                        << ch.get();
                    throw std::runtime_error(oss.str());
                }
            }

        private:
            Circle_proxy::Circle_adder _ca;
    };
}

static const Kernel::FT RAND_AMP_DEFAULT = 20;

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

    // Generate spheres
    std::cout << "Generating spheres" << std::endl;
    std::generate_n(std::back_inserter(spheres), nb_spheres,
            internal::Random_sphere_3(rand_amp));

    // Print out generated spheres
    //std::cout << "Generated spheres :" << std::endl;
    //for (std::vector<Sphere_3>::size_type i = 0; i < spheres.size(); i++)
    //{ std::cout << "[" << i << "] " << spheres[i] << std::endl; }

    // Do intersections
    std::cout << "Computing intersections" << std::endl;
    std::vector<Circle_3> intersect_circles;
    for (std::vector<Sphere_3>::const_iterator it = spheres.begin();
            it != spheres.end(); it++)
    {
        // Sphere to intersect
        const Sphere_3 & s1 = *it;

        for (std::vector<Sphere_3>::const_iterator it2 = (it + 1);
                it2 != spheres.end(); it2++)
        {
            // Sphere to intersect with
            const Sphere_3 & s2 = *it2;

            // Ignore self intersecting
            if (s1 == s2) { continue; }

            // Try intersection
            Object obj = intersection(s1, s2);

            // Only handle circle intersections
            if (obj.is_empty())
            { continue; }
            else if (const Circle_3 * c = object_cast<Circle_3>(&obj))
            { intersect_circles.push_back(*c); }
            else if (const Point_3 * p = object_cast<Point_3>(&obj))
            {
                // TODO
            }
        }
    }

    // Add intersections to proxy
    std::cout << "Adding intersection spheres to proxy" << std::endl;
    std::for_each(intersect_circles.begin(), intersect_circles.end(),
            internal::Add_and_check_circle(cp));
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
        std::cerr << usage() << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::logic_error e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
