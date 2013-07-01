#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

#include <QGLViewer/frame.h>

#include <CGAL/Random.h>

#include "sphereintersecter.h"
#include "sphereview.h"

namespace Ui {
    class MainWindow;
}

class WindowStateWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef SphereIntersecter SI;
    typedef SphereIntersecterKernel Kernel;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void addWindowStateWidget(WindowStateWidget *stateWidget);
    void setWindowStateWidget(WindowStateWidget *stateWidget);

    QList<SphereView>& spheres()
    { return sphereList; }
    const QList<SphereView>& spheres() const
    { return sphereList; }

    SphereView& sphere(int index)
    { return sphereList[index]; }
    const SphereView& sphere(int index) const
    { return sphereList.at(index); }

    const SphereView& addSphere(const SI::Sphere_handle &sh);
    void removeSphere(int index);

    QMenu *menubar() const;
    QWidget *sidebar() const;

    void showStatus(const QString &status);

    int statusTimeout() const
    { return statusTime; }
    void setStatusTimeout(int t)
    { statusTime = t; }

signals:
    void windowStateChanged();

private slots:
    void drawViewer();

private:
    // Displayed spheres list
    QList<SphereView> sphereList;

    // Window state widgets
    QList<WindowStateWidget> windowStateWidgets;

    // Status timeout (ms)
    int statusTime;

    // UI
    Ui::MainWindow *ui;

    // Random
    CGAL::Random randgen;
};

#endif // MAINWINDOW_H
