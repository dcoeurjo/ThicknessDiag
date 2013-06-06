#ifndef CGAL_GEOMVIEW_STREAM_EXTENSION_H
#define CGAL_GEOMVIEW_STREAM_EXTENSION_H

#include <CGAL/IO/Geomview_stream.h>

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
            Geomview_panel::Identifier panel, bool enabled);
} // namespace CGAL

#endif
