#include <iostream>
#include <algorithm>

#include <ctime>
#include <cstdlib>

#include <CGAL/Linear_cell_complex.h>
#include <CGAL/Linear_cell_complex_operations.h>

#include <CGAL/IO/Geomview_stream.h>
#include <CGAL/IO/Color.h>

#include "Geomview_stream_extension.h"

// Typedefs for template instanciation/shortcuts
typedef CGAL::Linear_cell_complex<3> LCC;
typedef LCC::Dart_handle Dart_handle;
typedef LCC::Point Point;

//// Global Geomview stream
//static CGAL::Geomview_stream gv;
//
//namespace internal {
//    // Generates a random (r, g, b) color, assuming the seed
//    // is already initialized for std::rand
//    CGAL::Color random_color()
//    {
//        return CGAL::Color(std::rand() % 255, std::rand() % 255,
//                std::rand() % 255);
//    }
//
//    // Convenience functor for displaying an LCC's darts to
//    // the CGAL Geomview visualizer
//    struct geomview_display_dart {
//        // Remove old objects, initialize reference to lcc
//        geomview_display_dart(const LCC & lcc):
//            lcc(lcc) { gv.clear(); }
//
//        // Recenter view on all objects (displayed darts)
//        ~geomview_display_dart()
//            { gv.look_recenter(); }
//
//        // Draw a dart, with a random color
//        void operator()(const LCC::Dart & d)
//        {
//            gv << random_color()
//                << LCC::point(lcc.dart_handle(d));
//        }
//
//        // Reference (const) to Linear Cell Complex
//        const LCC & lcc;
//    };
//}

// Overload to display a LCC's characteristics to an ostream
std::ostream & operator<<(std::ostream & os, const LCC & lcc)
{
    lcc.display_characteristics(os);
    return os;
}

int main(int argc, const char * argv[])
{
    //// Setup Geomview
    //gv.set_wired(false);
    //gv.set_bg_color(CGAL::Color(0, 0, 0));
    //gv.clear();
    //CGAL::set_panel_enabled(gv, CGAL::Geomview_panel::TOOLS, false);
    //CGAL::set_panel_enabled(gv, CGAL::Geomview_panel::GEOMVIEW, false);

    // Initialize random seed
    std::srand(std::time(0));

    // Linear cell complex
    LCC lcc;
    std::cout << "Linear Cell Complex" << std::endl
        << lcc << std::endl;

    // Adding first tetrahedron
    std::cout << "Adding first tetrahedron" << std::endl;
    Dart_handle dh1 = lcc.make_tetrahedron(Point(-1, 0, 0), Point(0, 2, 0),
            Point(1, 0, 0), Point(1, 1, 2));
    std::cout << lcc << std::endl;

    //// Adding second tetrahedron
    //std::cout << "Adding second tetrahedron" << std::endl;
    //Dart_handle dh2 = lcc.make_tetrahedron(Point(0, 2, -1), Point(-1, 0, -1),
    //        Point(1, 0, -1), Point(1, 1, -3));
    //std::cout << lcc << std::endl;

    //// Displaying first tetrahedron
    //std::cout << "Displaying first tetrahedron" <<  std::endl;
    //std::for_each(LCC::One_dart_per_incident_cell_const_range<0, 3>(lcc, dh1).begin(),
    //        LCC::One_dart_per_incident_cell_const_range<0, 3>(lcc, dh1).end(),
    //        internal::geomview_display_dart(lcc));
    //while (std::cin.get() != '\n');

    //// Displaying second tetrahedron
    //std::cout << "Displaying second tetrahedron" <<  std::endl;
    //std::for_each(LCC::One_dart_per_incident_cell_const_range<0, 3>(lcc, dh2).begin(),
    //        LCC::One_dart_per_incident_cell_const_range<0, 3>(lcc, dh2).end(),
    //        internal::geomview_display_dart(lcc));
    //while (std::cin.get() != '\n');

    //// Sewing the tetrahedra
    //std::cout << "Sewing the tetrahedra" << std::endl;
    //lcc.sew<3>(dh1, dh2);
    //std::cout << lcc << std::endl;

    // Insert 2-cell barycenter in the first tetrahedron
    std::cout << "Inserting 2-cell barycenter in the first tetrahedron" << std::endl;
    Dart_handle dh3 = lcc.insert_barycenter_in_cell<2>(dh1);
    std::cout << lcc << std::endl;

    // Linear_cell_complex should still be valid at the end
    CGAL_assertion(lcc.is_valid());
    return EXIT_SUCCESS;
}
