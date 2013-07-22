#include <boost/test/included/prg_exec_monitor.hpp>

#include <CGAL/assertions.h>
#include <CGAL/Exact_spherical_kernel_3.h>

#include "../Event_queue.h"
#include "../Event_queue_builder.h"
#include "../Sphere_intersecter.h"

typedef CGAL::Exact_spherical_kernel_3 SK;
typedef typename SK::Point_3 Point_3;
typedef typename SK::Sphere_3 Sphere_3;

typedef Sphere_intersecter<SK> SI;

typedef Event_queue<SK> EQ;
typedef Event_queue_builder<SK> EQB;
typedef typename EQ::Events Events;

int cpp_main(int argc, char * argv[])
{
    // Sphere intersecter instance holding actual spheres
    SI si;

    // Main sphere to work with
    SI::Sphere_handle sh = si.add_sphere(Sphere_3(Point_3(0, 0, 0), 49));

    // Add the spheres
    si.add_sphere(Sphere_3(Point_3(7, 0, 0, 25)));

    // Build the queue
    EQ eq = EQB()(si, sh);

    // Basic checks on event queue
    CGAL_assertion(eq.empty() == false);
    CGAL_assertion(eq.size() == 2);

    return EXIT_SUCCESS;
}
