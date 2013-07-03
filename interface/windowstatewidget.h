#ifndef WINDOWSTATEWIDGET_H
#define WINDOWSTATEWIDGET_H

#include <map>
#include <QWidget>
#include <QString>

#include <QGLViewer/qglviewer.h>

#include "sphereview.h"
#include "sphereintersecterproxy.h"
#include "windowstatefactory.h"

class Window;
class WindowState;

class WindowStateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WindowStateWidget(Window *window);

    // Access to sphere intersecter proxy
    const SphereIntersecterProxy& siProxy() const
    { return siProxyInstance; }
    SphereIntersecterProxy& siProxy()
    { return siProxyInstance; }

    // Set the status (~statusbar)
    void setStatus(const QString &status);

    // Get a builder bound to this widget
    WindowStateFactory factory();

private slots:
    // Slots for adding/removing spheres
    void onSphereAdded(const SphereHandle &sh);
    void onSphereRemoved(const SphereHandle &sh);

private:
    // Get the bound window (direct parent)
    Window& w()
    { return reinterpret_cast<Window&>(*parent()); }

    // Sphere display
    std::map<SphereHandle, SphereView> sphereViews;

    // Scene display viewport
    QGLViewer *viewer;

    // Sphere intersecter proxy
    SphereIntersecterProxy &siProxyInstance;

    // Current state (used for update)
    WindowState *state;
};

#endif // WINDOWSTATEWIDGET_H
