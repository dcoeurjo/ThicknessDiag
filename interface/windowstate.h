#ifndef WINDOWSTATE_H
#define WINDOWSTATE_H

#include <QObject>
#include <QString>

#include "windowstatewidget.h"

class WindowState : public QObject
{
    Q_OBJECT
public:
    explicit WindowState(WindowStateWidget &wsw);
    virtual ~WindowState();

public slots:
    void enterState();
    void leaveState();

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

    // Helper for setting the status
    void setStatus(const QString &status);
};

#endif // WINDOWSTATE_H
