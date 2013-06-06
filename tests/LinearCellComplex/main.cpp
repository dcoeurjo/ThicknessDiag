#include <iostream>
#include <algorithm>

#include <ctime>
#include <cstdlib>

#include <CGAL/Linear_cell_complex.h>
#include <CGAL/Linear_cell_complex_operations.h>

#include <CGAL/IO/Geomview_stream.h>
#include <CGAL/IO/Color.h>

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
        typedef CGAL::Segment_3<CGAL::Epick> Segment;

        Display_Vertices(const LCC & alcc):
            _lcc(alcc), _dart_id(0)
        {
            // Remove old objects
            gv.clear();
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
            typedef LCC::One_dart_per_incident_cell_const_range<0, 3> dart_range;
            dart_range it_range = _lcc.one_dart_per_incident_cell<0, 3>(_lcc.dart_handle(d));
            dart_range::const_iterator it = it_range.begin(),
                itend = it_range.end();
            while (it != itend)
            {
                Point start(LCC::point(it));
                //Point start(it);
                //gv << start;
                if (++it != itend)
                {
                    LCC::Point end(LCC::point(it));
                    //gv << end;
                    Segment segment(start, end);
                    gv << segment;
                }
            }
            gv.look_recenter();

            // Wait until next display
            while (std::cin.get() != '\n');
        }

    private:
        const LCC & _lcc;
        std::size_t _dart_id;
};

// Display the vertices of each volume by iterating on darts.
void display_all_vertices(const LCC_3 & lcc)
{
    std::for_each(lcc.one_dart_per_cell<3>().begin(),
            lcc.one_dart_per_cell<3>().end(),
            Display_Vertices(lcc));
}

// Small extension to CGAL for the Geomview_stream module
namespace CGAL {
    // Taken directly from the Geomview official website
    // (see geomview.org/docs/html/ui_002dpanel.html#ui_002dpanel)
    struct Geomview_panel {
        enum Identifier {
            GEOMVIEW,   // main panel
            TOOLS,      // motion controls
            APPEARANCE, // appearance controls
            CAMERAS,    // camera controls
            LIGHTING,   // lighting controls
            OBSCURE,    // obscure controls (doesn't seem to exist any more)
            MATERIALS,  // material properties controls
            COMMAND,    // command entry box
            CREDITS,    // geomview credits 
        };
    };

    // Extension allowing enable/disable for Geomview panels
    void set_panel_enabled(Geomview_stream &gv,
            Geomview_panel::Identifier panel, bool enabled)
    {
        gv << "(ui-panel ";
        switch (panel) {
            case Geomview_panel::GEOMVIEW:   gv << "geomview";   break;
            case Geomview_panel::TOOLS:      gv << "tools";      break;
            case Geomview_panel::APPEARANCE: gv << "appearance"; break;
            case Geomview_panel::CAMERAS:    gv << "cameras";    break;
            case Geomview_panel::LIGHTING:   gv << "lighting";   break;
            case Geomview_panel::OBSCURE:    gv << "obscure";    break;
            case Geomview_panel::MATERIALS:  gv << "materials";  break;
            case Geomview_panel::COMMAND:    gv << "command";    break;
            case Geomview_panel::CREDITS:    gv << "credits";    break;
        }
        gv << " ";
        if (enabled) {
            gv << "on";
        } else {
            gv << "off";
        }
        gv << ")";
    }
} // namespace CGAL

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
    Dart_handle d1 = lcc.make_tetrahedron(Point(-1, 0, 0), Point(0, 2, 0),
            Point(1, 0, 0), Point(1, 1, 2));
    Dart_handle d2 = lcc.make_tetrahedron(Point(0, 2, -1),
            Point(-1, 0, -1),
            Point(1, 0, -1),
            Point(1, 1, -3));
    std::cout << "Adding two tetrahedra" << std::endl;
    display_all_vertices(lcc);

    // 3-Sew the 2 tetrahedra along one facet
    lcc.sew<3>(d1, d2);
    std::cout << "Sewing tetrahedra" << std::endl;
    display_all_vertices(lcc);

    // Translate the second tetrahedra by a given vector
    LCC_3::Vector v(3, 1, 1);
    for (LCC_3::One_dart_per_incident_cell_range<0, 3>::iterator
            it = lcc.one_dart_per_incident_cell<0, 3>(d2).begin(),
            itend = lcc.one_dart_per_incident_cell<0, 3>(d2).end();
            it != itend; ++it)
    {
        LCC_3::point(it) = LCC_3::Traits::Construct_translated_point_3()
            (LCC_3::point(it), v);
    }
    std::cout << "Translating the second tetrahedra" << std::endl;
    display_all_vertices(lcc);

    return EXIT_SUCCESS;
}
