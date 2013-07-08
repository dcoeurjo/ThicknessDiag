#include "customviewer.h"

CustomViewer::CustomViewer(QWidget *parent):
    QGLViewer(parent) {}

void CustomViewer::init()
{
    // Disable shortcuts, comment to enable
    //setShortcut(DRAW_AXIS, 0);
    //setShortcut(DRAW_GRID, 0);
    //setShortcut(DISPLAY_FPS, 0);
    setShortcut(ENABLE_TEXT, 0);
    setShortcut(EXIT_VIEWER, 0);
    setShortcut(SAVE_SCREENSHOT, 0);
    //setShortcut(CAMERA_MODE, 0);
    setShortcut(FULL_SCREEN, 0);
    setShortcut(STEREO, 0);
    setShortcut(ANIMATION, 0);
    setShortcut(HELP, 0);
    setShortcut(EDIT_CAMERA, 0);
    //setShortcut(MOVE_CAMERA_LEFT, 0);
    //setShortcut(MOVE_CAMERA_RIGHT, 0);
    //setShortcut(MOVE_CAMERA_UP, 0);
    //setShortcut(MOVE_CAMERA_DOWN, 0);
    setShortcut(INCREASE_FLYSPEED, 0);
    setShortcut(DECREASE_FLYSPEED, 0);
    setShortcut(SNAPSHOT_TO_CLIPBOARD, 0);
}
