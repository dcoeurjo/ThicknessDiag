#ifndef WINDOWSTATE_H
#define WINDOWSTATE_H

#include <QObject>
#include <QString>

#include "windowstatewidget.h"

class QAction;
class QGLViewer;

class WindowState : public QObject
{
    Q_OBJECT
public:
    explicit WindowState(WindowStateWidget &wsw, const QString &n);
    virtual ~WindowState();

    // Used by widget for notification
    void enterState();
    void leaveState();

    //Used by widget for setup at each enter
    // (override in subclasses)
    virtual void setup() {}

    // Used to display the widget regularly
    virtual void draw() {}

    // Get/Set the state's name
    const QString& name() const
    { return nameVal; }
    void setName(const QString &name)
    { nameVal = name; }

    // Make a new enter action and return it
    QAction* enterAction() const
    { return enterActionMember; }

public slots:
    void requestEnter()
    { wsw.changeState(*this); }

signals:
    void stateEntered();
    void stateLeft();

protected:
    // State logic, implemented in subclasses
    virtual void onEnterState() = 0;
    virtual void onLeaveState() = 0;

    // Sphere intersecter proxy
    SphereIntersecterProxy &siProxy;

    // Bound widget
    WindowStateWidget& wsw;

    // Helper for obtaining the main window (as a QMainWindow)
    QMainWindow& mw()
    { return wsw.mw(); }

    // Helper for setting the status
    void setStatus(const QString &status)
    { wsw.setStatus(status); }

private:
    QString nameVal;
    QAction *enterActionMember;
};

#endif // WINDOWSTATE_H
