#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "sphereintersecterproxy.h"

class QGridLayout;
class WindowStateWidget;

class Window : public QMainWindow
{
    Q_OBJECT
public:
     Window(QWidget *parent = 0);

    // Access to sphere intersecter proxy
    const SphereIntersecterProxy& siProxy() const
    { return siProxyMember; }
    SphereIntersecterProxy& siProxy()
    { return siProxyMember; }

private:
    // Window state widget
    WindowStateWidget *wsw;

    // Window UI
    QWidget *centralWidget;
    QGridLayout *gridLayout;

    // Sphere intersecter proxy
    SphereIntersecterProxy siProxyMember;
};

#endif // MAINWINDOW_H
