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

#ifdef DISPLAY_ON_GEOMVIEW
#  include <CGAL/IO/Geomview_stream.h>
#  include <CGAL/IO/Color.h>
#  include <Geomview_stream_extension.h>
#endif // DISPLAY_ON_GEOMVIEW //

#include <Sphere_intersecter.h>

// Faster kernel
typedef CGAL::Simple_cartesian<double> CK;
typedef CGAL::Algebraic_kernel_for_spheres_2_3<CK::RT> AKS;
typedef CGAL::Spherical_kernel_3<CK, AKS> SK;

// Exact kernel
typedef CGAL::Exact_spherical_kernel_3 ESK;

// Definition of actually used kernel
typedef ESK Kernel;

// Geometric objects
typedef typename Kernel::FT FT;
typedef typename Kernel::Point_3 Point_3;
typedef typename Kernel::Circle_3 Circle_3;
typedef typename Kernel::Sphere_3 Sphere_3;

// Random numbers generator
typedef CGAL::Random Random;

#ifdef DISPLAY_ON_GEOMVIEW // Geomview
typedef CGAL::Geomview_stream Geomview_stream;
typedef CGAL::Color Color;
typedef CGAL::Geomview_panel Geomview_panel;
using CGAL::set_panel_enabled;

Color random_color(Random & rand)
{ return Color(rand.get_int(0, 255),
    rand.get_int(0, 255),
    rand.get_int(0, 255)); }

class Display_sphere_on_geomview
{
  public:
    Display_sphere_on_geomview(Geomview_stream & gv):
      _gv(gv) {}

    void operator()(const Sphere_3 & sphere)
    { _gv << random_color(_rand) << sphere; }

  private:
    Geomview_stream & _gv;
    Random _rand;
};

#endif // DISPLAY_ON_GEOMVIEW //

typedef Sphere_intersecter<Kernel> SI;

class Random_sphere_3
{
  typedef std::vector<Sphere_3> Sphere_list;

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
    Sphere_list::const_iterator it = std::find(_spheres.begin(),
        _spheres.end(), s);
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
  Sphere_list _spheres;
};

static const Kernel::FT RAND_AMP_DEFAULT = 100;

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

#ifdef DISPLAY_ON_GEOMVIEW
  // Setup geomview
  Geomview_stream gv;
  gv.set_wired(false);
  gv.set_bg_color(Color(0, 0, 0));
  gv.clear();
  set_panel_enabled(gv, Geomview_panel::TOOLS, false);
  set_panel_enabled(gv, Geomview_panel::GEOMVIEW, false);
#endif // DISPLAY_ON_GEOMVIEW //

  // Random amplitude for sphere generation
  FT rand_amp = RAND_AMP_DEFAULT;
  if (argc > 2)
  {
    // Too many arguments
    if (argc != 3)
    { throw std::runtime_error("Too many arguments specified"); }

    // Convert rand_amp
    std::istringstream iss(argv[2]);
    if ( (iss >> rand_amp) == false )
    { throw std::runtime_error("Bad number format (real number)"); }
  }

  // Circle proxy to test
  SI si;

  // Generate spheres
  //std::cout << "Generating spheres..." << std::flush;
  std::generate_n(si.insert_iterator(), nb_spheres,
      Random_sphere_3(rand_amp));
  //std::cout << "done." << std::endl;

#ifdef DISPLAY_ON_GEOMVIEW
  // Show added spheres on Geomview
  //std::cout << "Displaying spheres on Geomview..." << std::flush;
  SI::Sphere_iterator_range spheres = si.spheres();
  std::for_each(spheres.begin(), spheres.end(),
      Display_sphere_on_geomview(gv));
  gv.look_recenter();
  //std::cout << "done." << std::endl;
  while (std::cin.get() != '\n');
#endif // DISPLAY_ON_GEOMVIEW //
}

int main(int argc, const char * argv[])
{
  try
  {
    do_main(argc, argv);
    return EXIT_SUCCESS;
  }
  catch (std::logic_error e)
  {
    std::cerr << "Usage: circle_proxy NB_SPHERES [RAND_AMP="
      << RAND_AMP_DEFAULT << "]" << std::endl
      << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch (std::runtime_error e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}

// vim: sw=2 et ts=2 sts=2
