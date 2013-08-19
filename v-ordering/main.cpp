#include <vector>
#include <iterator>
#include <CGAL/Exact_spherical_kernel_3.h>
#include <Sphere_intersecter.h>

typedef CGAL::Exact_spherical_kernel_3 SK;

typedef SK::Point_3 Point_3;
typedef SK::Circle_3 Circle_3;
typedef SK::Sphere_3 Sphere_3;

typedef Sphere_intersecter<SK>::Circle_handle Circle_handle;
typedef Sphere_intersecter<SK>::Sphere_handle Sphere_handle;

// Test circles (ugly, but more reliable than a .txt file)
static const double test_spheres[][4] = {
#  include "test_spheres.h"
};
// and the size of this array
static const std::size_t test_circles_size = sizeof(test_spheres) / sizeof(double[4]);

// Main sphere to work with
static const double test_sphere[4] = { 0, 0, 0, 2 };

int main(int argc, const char * argv[])
{
  // 1) Setup initial spheres
  Sphere_intersecter<SK> si;
  // add spheres to SI
  for (std::size_t i = 0; i < test_circles_size; i++)
  {
    si.add_sphere(Sphere_3(
          Point_3(test_spheres[i][0], test_spheres[i][1],
            test_spheres[i][2]),
          test_spheres[i][3]));
  }
  // add the sphere to work with
  Sphere_3 s(Point_3(test_sphere[0], test_sphere[1],
          test_sphere[2]), test_sphere[3]);
  Sphere_handle sh = si.add_sphere(s);
  // get the circles on our main sphere
  typedef std::vector<Circle_handle> Circle_handle_list;
  Circle_handle_list circles_on_s;
  si.circles_on_sphere(sh, std::back_inserter(circles_on_s));
  // display these circles
  std::cout << "Sphere: " << s << std::endl
    << "Circles:" << std::endl;
  for (Circle_handle_list::const_iterator it = circles_on_s.begin();
      it != circles_on_s.end(); it++)
  { std::cout << **it << std::endl; }

  // 2) Position the (virtual) meridian at an appropriate angle

  return 0;
}

// vim: sw=2 et ts=2 sts=2
