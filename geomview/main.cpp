#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

#include <Sphere_intersecter.h>
#include <Event_queue_builder.h>

#include <CGAL/Random.h>
typedef CGAL::Random Random;

#include <CGAL/Exact_spherical_kernel_3.h>
typedef CGAL::Exact_spherical_kernel_3 Kernel;

// Geometric objects
typedef typename Kernel::FT FT;
typedef typename Kernel::Line_3 Line_3;
typedef typename Kernel::Point_3 Point_3;
typedef typename Kernel::Plane_3 Plane_3;
typedef typename Kernel::Vector_3 Vector_3;
typedef typename Kernel::Circle_3 Circle_3;
typedef typename Kernel::Sphere_3 Sphere_3;
typedef typename Kernel::Segment_3 Segment_3;
typedef typename Kernel::Circular_arc_point_3 Circular_arc_point_3;

// Intersection objects
typedef typename Kernel::Object_3 Object_3;
typedef typename Kernel::Assign_3 Assign_3;
typedef typename Kernel::Intersect_3 Intersect_3;

// CGAL helpers
using CGAL::to_double;

// Sphere intersecter
typedef Sphere_intersecter<Kernel> SI;
typedef Event_queue<Kernel> EQ;
typedef Event_queue_builder<Kernel> EQB;

#ifdef DISPLAY_ON_GEOMVIEW
#  include <CGAL/IO/Geomview_stream.h>
#  include <CGAL/IO/Color.h>
#  include <Geomview_stream_extension.h>

typedef CGAL::Geomview_stream Geomview_stream;
typedef CGAL::Color Color;
typedef CGAL::Geomview_panel Geomview_panel;
using CGAL::set_panel_enabled;

inline Color random_color(Random & rand)
{ return Color(rand.get_int(0, 255),
      rand.get_int(0, 255),
      rand.get_int(0, 255)); }

inline Point_3 to_point_3(const Circular_arc_point_3 & cap)
{ return Point_3(to_double(cap.x()),
      to_double(cap.y()), to_double(cap.z())); }

class Display_on_gv
{
  public:
    Display_on_gv(Geomview_stream & gv):
      _gv(gv), _rand() {}

    Geomview_stream & operator()()
    { return _gv << random_color(_rand); }

  private:
    Geomview_stream & _gv;
    Random _rand;
};

struct Display_sphere_on_gv: Display_on_gv
{
  Display_sphere_on_gv(Geomview_stream & gv):
    Display_on_gv(gv) {}

  Geomview_stream & operator()(const SI::Sphere_handle & sh)
  { return Display_on_gv::operator()() << *sh; }
};

struct Display_circle_on_gv: Display_on_gv
{
  Display_circle_on_gv(Geomview_stream & gv):
    Display_on_gv(gv) {}

  Geomview_stream & operator() (const SI::Circle_handle & ch)
  {
    // Get axes
    Plane_3 plane = ch->supporting_plane();
    Line_3 line_theta(ch->center(), plane.base1()),
           line_phi(ch->center(), plane.base2());

    // Get intersection points
    Object_3 c_theta_points[2], c_phi_points[2];
    Intersect_3()(*ch, line_theta, c_theta_points);
    Intersect_3()(*ch, line_phi, c_phi_points);

    // Convert all points
    typedef std::pair<Circular_arc_point_3, unsigned int> CAP;
    CAP cap_theta[2], cap_phi[2];
    Assign_3()(cap_phi[0], c_phi_points[0]);
    Assign_3()(cap_phi[1], c_phi_points[1]);
    Assign_3()(cap_theta[0], c_theta_points[0]);
    Assign_3()(cap_theta[1], c_theta_points[1]);

    return Display_on_gv::operator()()
      << Segment_3(to_point_3(cap_phi[0].first), to_point_3(cap_theta[0].first))
      << Segment_3(to_point_3(cap_phi[0].first), to_point_3(cap_theta[1].first))
      << Segment_3(to_point_3(cap_phi[1].first), to_point_3(cap_theta[0].first))
      << Segment_3(to_point_3(cap_phi[1].first), to_point_3(cap_theta[1].first));
  }
};

struct Display_cap_on_gv: Display_on_gv
{
  Display_cap_on_gv(Geomview_stream & gv):
    Display_on_gv(gv) {}

  Geomview_stream & operator()(const Circular_arc_point_3 & cap)
  { return Display_on_gv::operator()() << to_point_3(cap); }
};

#endif // DISPLAY_ON_GEOMVIEW //

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
      { return (*this)(); }
      _spheres.push_back(s);
      return _spheres.back();
    }

  private:
    FT _amp;
    Random _rand;
    Sphere_list _spheres;
};

class usage_error: public std::runtime_error
{
  public:
    explicit usage_error(const std::string & what_arg):
      std::runtime_error(what_arg) {}

    explicit usage_error(const char * what_arg):
      std::runtime_error(what_arg) {}
};

static void do_main(const std::size_t & nb_spheres, const FT & rand_amp)
{
#ifdef DISPLAY_ON_GEOMVIEW // Setup gv
  Geomview_stream gv;
  gv.set_wired(false);
  gv.set_bg_color(Color(0, 0, 0));
  gv.clear();
  set_panel_enabled(gv, Geomview_panel::TOOLS, false);
  set_panel_enabled(gv, Geomview_panel::GEOMVIEW, false);
#endif // DISPLAY_ON_GEOMVIEW //

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
      Display_sphere_on_gv(gv));
  gv.look_recenter();
  //std::cout << "done." << std::endl;
  while (std::cin.get() != '\n');
#endif // DISPLAY_ON_GEOMVIEW //

  // Sphere to apply BO on
  Sphere_3 s(Point_3(0, 0, 0), 42);
  SI::Sphere_handle sh = si.add_sphere(s);
#ifdef DISPLAY_ON_GEOMVIEW
  // Function objects
  Display_cap_on_gv display_cap(gv);
  Display_sphere_on_gv display_sphere(gv);
  Display_circle_on_gv display_circle(gv);

//  // Display sphere
//  gv.clear();
//  display_sphere(sh);
//
//  // Display circles on sphere
//  std::vector<SI::Circle_handle> s_circles;
//  si.circles_on_sphere(sh, std::back_inserter(s_circles));
//  std::for_each(s_circles.begin(), s_circles.end(),
//      Display_circle_on_gv(gv));
#endif // DISPLAY_ON_GEOMVIEW //

  // Event queue filling
  EQ ev_queue = EQB()(si, sh);

  // Empty queue ?
  if (ev_queue.empty())
  { std::cout << "No events pushed to queue" << std::endl; }
  // Pop-and-display events
  for (EQ::Event_site_type ev_type = ev_queue.next_event();
      ev_type != EQ::None; ev_type = ev_queue.next_event())
  {
    if (ev_type == EQ::Normal)
    {
      typedef EQ::Normal_event_site NES;
      NES nes = ev_queue.pop_normal();
#ifdef DISPLAY_ON_GEOMVIEW
      NES::Intersection_events_range ie_range(nes);
      for (NES::Intersection_events_iterator it = ie_range.begin();
          it != ie_range.end(); it++)
      {
        typedef EQ::Events::Intersection_event IE;
        const IE & ie = *it;

        // Intersection event circles
        SI::Circle_handle ch1 = ie.circles.first, ch2 = ie.circles.second;
        CGAL_assertion(ch1.is_null() == false);
        CGAL_assertion(ch2.is_null() == false);

        // Circle source spheres
        SI::Sphere_handle_pair shp1 = si.originating_spheres(ch1),
                           shp2 = si.originating_spheres(ch2);
        CGAL_assertion(shp1.first == sh || shp1.second == sh);
        CGAL_assertion(shp2.first == sh || shp2.second == sh);

        // Display each circle
        gv.clear();
        display_cap(nes.point());
        display_circle(ch1);
        //display_sphere(shp1.first);
        //display_sphere(shp1.second);
        display_circle(ch2);
        //display_sphere(shp2.first);
        //display_sphere(shp2.second);
        while (std::cin.get() != '\n');
      }
#endif // DISPLAY_ON_GEOMVIEW //
    }
    else if (ev_type == EQ::Polar)
    {
      typedef EQ::Polar_event_site PES;
      PES pes = ev_queue.pop_polar();
      // TODO
    }
    else
    {
      CGAL_assertion(ev_type == EQ::Bipolar);
      typedef EQ::Bipolar_event_site BPES;
      BPES bpes = ev_queue.pop_bipolar();
      // TODO
    }
  }
}

static const std::size_t NB_SPHERES_DEFAULT = 200;
static const FT RAND_AMP_DEFAULT = 100;

int main(int argc, const char * argv[])
{
  try
  {
    // Amount of spheres to intersect
    std::size_t nb_spheres = NB_SPHERES_DEFAULT;

    // Convert nb_spheres
    if (argc > 1)
    { std::istringstream iss(argv[1]);
      if ( (iss >> nb_spheres).fail() )
      { throw usage_error("Bad number format (positive integer)"); } }

    // Random amplitude for sphere generation
    FT rand_amp = RAND_AMP_DEFAULT;
    if (argc > 2)
    {
      // Too many arguments
      if (argc != 3)
      { throw usage_error("Too many arguments specified"); }

      // Convert rand_amp
      std::istringstream iss(argv[2]);
      if ( (iss >> rand_amp).fail() )
      { throw usage_error("Bad number format (real number)"); }
    }
    do_main(nb_spheres, rand_amp);
    return EXIT_SUCCESS;
  }
  catch (usage_error e)
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
