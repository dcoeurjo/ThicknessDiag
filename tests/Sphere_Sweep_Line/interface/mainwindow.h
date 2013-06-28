#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <CGAL/Exact_spherical_kernel_3.h>
#include <Sphere_intersecter.h>
#include <QGLViewer/frame.h>

namespace Ui {
    class MainWindow;
}

namespace CGAL {
    class Random;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    typedef CGAL::Exact_spherical_kernel_3 Kernel;
    typedef Sphere_intersecter<Kernel> SI;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // Sphere list slots
    void addNewSphere();
    void deleteSelectedSpheres();
    void toggleAllSpheresCheckState();
    void showSpheresMenuAt(const QPoint &point);

    // File menu slots
    void openSpheres();
    void saveSpheres();
    void saveSpheresAs();
    void generateSpheres();

    // Other slots
    void drawSpheres();

private:
    // View of a sphere
    struct Sphere_view
    {
        SI::Sphere_handle handle;
        QColor color;
        qglviewer::Frame frame;
        double radius;
    };

    // Helpers
    void addNewSphere(const SI::Sphere_handle &sh);
    void drawSphere(const Sphere_view &sv);

    // Sphere intersecter object (main purpose)
    SI si;

    // Displayed sphere list
    typedef std::vector<Sphere_view> SphereList;
    SphereList sphereViewList;

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
