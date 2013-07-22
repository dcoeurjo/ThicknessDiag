#ifndef WINDOWSTATEWIDGET_H
#define WINDOWSTATEWIDGET_H

#include <set>
#include <QWidget>
#include <QString>
#include "windowstatefactory.h"
#include "../sphereview.h"
#include "../sphereintersecterproxy.h"

class QMenu;
class QGLViewer;
class QMainWindow;

class WindowState;

class WindowStateWidget : public QWidget
{
    Q_OBJECT
public:
     WindowStateWidget(SphereIntersecterProxy &siProxy,
                               QMainWindow *window = 0);
    virtual ~WindowStateWidget();

    // Access to sphere intersecter proxy
    const SphereIntersecterProxy& siProxy() const
    { return siProxyInstance; }
    SphereIntersecterProxy& siProxy()
    { return siProxyInstance; }

    // Set the status (~statusbar)
    void setStatus(const QString &status, int timeout = 5000);

    // Used for switching state from an external state
    void changeState(WindowState &state);

    // Used to add menu actions for states
    void addState(WindowState &state);
    // shortcut to use a state id
    void addState(WindowStateFactory::StateId &stateId)
    { addState(factory().makeState(stateId)); }

    // Get a state factory bound to this widget
    WindowStateFactory factory()
    { return WindowStateFactory(*this); }

    // Get the view bound to a sphere
    const SphereView& sphereView(const SphereHandle& sh);

    // Get the viewer
    QGLViewer *viewer() const
    { return viewerMember; }

    // Get the bound window (direct parent)
    QMainWindow& mw()
    { return reinterpret_cast<QMainWindow&>(*window()); }

protected slots:
    // Slots for adding/removing spheres
    void onSphereAdded(const Sphere_3 &s);
    void onSphereRemoved(const Sphere_3 &s);

private slots:
    // Slot for drawing the viewer
    void drawViewer();

private:
    // Sphere display
    typedef std::set<SphereView> SphereViews;
    SphereViews sphereViews;

    // UI
    QGLViewer *viewerMember;
    QMenu *stateMenu;

    // Sphere intersecter proxy
    SphereIntersecterProxy &siProxyInstance;

    // Current state (used for update)
    WindowState *currentState;
    // nb states added (used for shortcut setup)
    std::size_t nbStates;
};

#endif // WINDOWSTATEWIDGET_H
