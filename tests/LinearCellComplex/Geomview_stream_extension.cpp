#include "Geomview_stream_extension.h"

namespace CGAL {
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
}
