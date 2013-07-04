#ifndef WINDOWSTATEWIDGET_H
#define WINDOWSTATEWIDGET_H

#include <map>
#include <QWidget>
#include <QString>

#include <QGLViewer/qglviewer.h>

#include "sphereview.h"
#include "sphereintersecterproxy.h"
#include "windowstatefactory.h"

class QMenu;
class QMainWindow;

class WindowState;

class WindowStateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WindowStateWidget(SphereIntersecterProxy &siProxy,
                               QMainWindow *window = 0);
    virtual ~WindowStateWidget();

    // Access to sphere intersecter proxy
    const SphereIntersecterProxy& siProxy() const
    { return siProxyInstance; }
    SphereIntersecterProxy& siProxy()
    { return siProxyInstance; }

    // Set the status (~statusbar)
    void setStatus(const QString &status, int timeout = 5000);

    // Get a builder bound to this widget
    WindowStateFactory factory();

    // Used for switching state from an external state
    void changeState(WindowState &state);

    // Used to add menu actions for states
    void addState(WindowState &state);

    // Get the view bound to a sphere
    const SphereView& sphereView(const SphereHandle& sh);

protected slots:
    // Slots for adding/removing spheres
    void onSphereAdded(const SphereHandle &sh);
    void onSphereRemoved(const SphereHandle &sh);

private slots:
    // Slot for drawing the viewer
    void drawViewer();

private:
    // Get the bound window (direct parent)
    QMainWindow& mw()
    { return reinterpret_cast<QMainWindow&>(*window()); }

    // Sphere display
    typedef std::map<SphereHandle, SphereView> SphereViews;
    SphereViews sphereViews;

    // UI
    QGLViewer *viewer;
    QMenu *stateMenu;

    // Sphere intersecter proxy
    SphereIntersecterProxy &siProxyInstance;

    // Current state (used for update)
    WindowState *currentState;
};

#endif // WINDOWSTATEWIDGET_H
