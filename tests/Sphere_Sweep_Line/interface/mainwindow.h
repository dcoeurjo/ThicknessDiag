#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

#include <QGLViewer/frame.h>

#include "sphereintersecter.h"

namespace Ui {
    class MainWindow;
}

namespace CGAL {
    class Random;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef SphereIntersecter SI;
    typedef SphereIntersecterKernel Kernel;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum Mode {
        SPHERES, EVENT_QUEUE
    };

private slots:
    // Sphere list slots
    void addNewSphere();
    void enterSpheresMode();
    void deleteSelectedSpheres();
    void toggleAllSpheresCheckState();
    void showSpheresMenuAt(const QPoint &point);

    // Event queue slots
    void buildEventQueue();
    void enterEventQueueMode();

    // File menu slots
    void openSpheres();
    void saveSpheres();
    void saveSpheresAs();
    void generateSpheres();

    // Other slots
    void drawViewer();

signals:
    void modeEntered(const Mode &m);

private:
    // View of a sphere
    struct SphereView
    {
        SphereView();
        SphereView(const SI::Sphere_handle &sh, const QColor &color,
                   const qglviewer::Vec &pos, double radius);

        SI::Sphere_handle handle;
        QColor color;
        qglviewer::Frame frame;
        double radius;
    };

    // Helpers
    void drawSpheres();
    void drawSphere(const SphereView &sv);
    void addNewSphere(const SI::Sphere_handle &sh, bool updateUI = true);
    void showStatusMessage(const QString &msg);
    void changeMode(const Mode &m);

    // Static helpers
    static QString sphereString(const Kernel::Sphere_3 &s);
    static QString sphereString(const SI::Sphere_handle &sh);

    // Window mode
    Mode mode;

    // Sphere intersecter object (main purpose)
    SI si;

    // Displayed sphere list
    typedef QVector<SphereView> SphereList;
    SphereList sphereList;

    // File opened (default "save" file)
    QString openFilename;

    // Saved check state for list widget
    Qt::CheckState savedCheckState;

    // UI
    Ui::MainWindow *ui;

    // Random number generator
    CGAL::Random *random;
};

#endif // MAINWINDOW_H
