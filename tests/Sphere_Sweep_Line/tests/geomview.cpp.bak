#include <iostream>
#include <algorithm>

#include <ctime>
#include <cstdlib>

#include <CGAL/Linear_cell_complex.h>
#include <CGAL/Linear_cell_complex_operations.h>

#include <CGAL/IO/Geomview_stream.h>
#include <CGAL/IO/Color.h>

#include "Geomview_stream_extension.h"

// Global Geomview stream
static CGAL::Geomview_stream gv;

// Typedefs for template instanciation/shortcuts
typedef CGAL::Linear_cell_complex<3> LCC_3;
typedef LCC_3::Dart_handle Dart_handle;
typedef LCC_3::Point Point;
typedef LCC_3::FT FT;

typedef CGAL::Color Color;

// Functor used to display all the vertices of a given volume.
class Display_Vertices
{
    public:
        typedef LCC_3 LCC;
        typedef LCC::Point Point;

        typedef CGAL::Epick Kernel;

        typedef Kernel::FT FT;
        typedef Kernel::RT RT;

        typedef CGAL::Line_3<Kernel> Line;
        typedef CGAL::Vector_3<Kernel> Vector;
        typedef CGAL::Segment_3<Kernel> Segment;
        typedef CGAL::Tetrahedron_3<Kernel> Tetrahedron;

        Display_Vertices(const LCC & alcc):
            _lcc(alcc), _dart_id(0)
        {
            // Remove old objects
            gv.clear();
        }

        static Vector rotate_vector_around_axis(const Vector & v,
                const Line & axis, FT angle)
        {
            CGAL_assertion(v.squared_length() == 1);
            // TODO
            return Vector();
        }

        void operator()(const LCC::Dart & d)
        {
            // Generate random color
            unsigned char r, g, b;
            r = 255 - (std::rand() % 255) / 2;
            g = 255 - (std::rand() % 255) / 2;
            b = 255 - (std::rand() % 255) / 2;
            gv << Color(r, g, b);

            // Display current dart
            std::cout << "DART #" << ++_dart_id << std::endl;
            typedef LCC::One_dart_per_incident_cell_const_range<0, 3>
                dart_range;
            dart_range it_range = _lcc.one_dart_per_incident_cell<0, 3>
                (_lcc.dart_handle(d));
            dart_range::const_iterator it = it_range.begin(),
                itend = it_range.end();
            while (it != itend)
            {
                // Start point
                Point start(LCC::point(it));
                if (++it != itend)
                {
                    // End point
                    LCC::Point end(LCC::point(it));

                    // Draw start->end segment
                    Segment segment(start, end);
                    gv << segment;

                    // Draw tetrahedron for arrow, composed of generated
                    // points plus the end point, two constants (W, H) are needed
                    FT w = 1, h = 1;
                    const std::size_t nb_corners = 5;
                    CGAL_static_assertion(nb_corners > 0);

                    // Vector defining the segment start->end, of length h
                    Vector v(segment.to_vector());
                    CGAL_assertion(v.squared_length() != 0);
                    v = v * h / v.squared_length();

                    // Other corners of the tetrahedron
                    Point corners[nb_corners] = {
                        Point(end + v),
                        Point(end + v),
                        Point(end + v),
                    };

                    // Add the normal translation to the segment, 
                    // rotated around this one by the correct angle
                    FT step = 360 / nb_corners;
                    Line axis = segment.supporting_line();
                    Vector base = axis.perpendicular_plane(corners[0]).base1();
                    for (std::size_t i = 0; i < nb_corners; i++)
                    {
                        FT angle = step*i;
                        Vector new_v = rotate_vector_around_axis(base, axis, angle);
                        corners[i] = corners[i] + new_v / h*new_v.squared_length();
                    }

                    // Draw the tetrahedron
                    gv << Tetrahedron(end, corners[0], corners[1], corners[2]);
                }
            }
            gv.look_recenter();
        }

    private:
        const LCC & _lcc;
        std::size_t _dart_id;
};

// Display the vertices of each volume by iterating on darts.
void display_all_vertices(const LCC_3 & lcc)
{
    // Display the vertices
    std::for_each(lcc.one_dart_per_cell<3>().begin(),
            lcc.one_dart_per_cell<3>().end(),
            Display_Vertices(lcc));

    // Wait until next display
    while (std::cin.get() != '\n');
}

int main(int argc, const char * argv[])
{
    // Setup Geomview
    gv.set_wired(false);
    gv.set_bg_color(Color(0, 0, 0));
    gv.clear();
    CGAL::set_panel_enabled(gv, CGAL::Geomview_panel::TOOLS, false);
    CGAL::set_panel_enabled(gv, CGAL::Geomview_panel::GEOMVIEW, false);

    // Initialize random seed
    std::srand(std::time(0));

    // Linear cell complex
    LCC_3 lcc;

    // Create two tetrahedra.
    Dart_handle dh1 = lcc.make_tetrahedron(Point(-1, 0, 0), Point(0, 2, 0),
            Point(1, 0, 0), Point(1, 1, 2));
    Dart_handle dh2 = lcc.make_tetrahedron(Point(0, 2, -1),
            Point(-1, 0, -1),
            Point(1, 0, -1),
            Point(1, 1, -3));
    std::cout << "Adding two tetrahedra" << std::endl;
    display_all_vertices(lcc);

    // 3-Sew the 2 tetrahedra along one facet
    lcc.sew<3>(dh1, dh2);
    std::cout << "Sewing tetrahedra" << std::endl;
    display_all_vertices(lcc);

    // Translate the second tetrahedra by a given vector
    LCC_3::Vector v(3, 1, 1);
    for (LCC_3::One_dart_per_incident_cell_range<0, 3>::iterator
            it = lcc.one_dart_per_incident_cell<0, 3>(dh2).begin(),
            itend = lcc.one_dart_per_incident_cell<0, 3>(dh2).end();
            it != itend; ++it)
    {
        LCC_3::point(it) = LCC_3::Traits::Construct_translated_point_3()
            (LCC_3::point(it), v);
    }
    std::cout << "Translating the second tetrahedra" << std::endl;
    display_all_vertices(lcc);

    // Insert a 0-cell in the first tetrahedron
    std::cout << "Insert a 0-cell in the first tetrahedron" << std::endl;
    CGAL_assertion(CGAL::is_insertable_cell_1_in_cell_2
            (lcc, dh1, dh1->beta(1)));
    Dart_handle dh3 = insert_cell_1_in_cell_2(lcc, dh1, dh1->beta(1));
    display_all_vertices(lcc);

    // Linear_cell_complex should still be valid at the end
    CGAL_assertion(lcc.is_valid());
    return EXIT_SUCCESS;
}
