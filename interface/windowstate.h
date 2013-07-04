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
    explicit WindowState(WindowStateWidget &wsw);
    virtual ~WindowState();

    // Used by widget for notification
    void enterState();
    void leaveState();

    //Used by widget for setup at each enter
    // (override in subclasses)
    virtual void setupWidget(QWidget*) {}

    // Used to display the widget regularly
    virtual void drawToViewer(QGLViewer *) {}

    // Get/Set the state's name
    const QString& name() const
    { return nameVal; }
    void setName(const QString &name)
    { nameVal = name; }

    // Make a new enter action and return it
    QAction* makeEnterAction();

public slots:
    void requestEnter();

signals:
    void stateEntered();
    void stateLeft();

protected:
    // State logic, implemented in subclasses
    virtual void onEnterState() = 0;
    virtual void onLeaveState() = 0;

    // Sphere intersecter proxy
    SphereIntersecterProxy &siProxy;

    // Helper for obtaining the bound widget
    WindowStateWidget& wsw()
    { return reinterpret_cast<WindowStateWidget&>(*parent());  }
    // helper for obtaining the main window (as a QMainWindow)
    QMainWindow& mw()
    { return reinterpret_cast<QMainWindow&>(*wsw().window()); }

    // Helper for setting the status
    void setStatus(const QString &status);

private:
    QString nameVal;
};

#endif // WINDOWSTATE_H
