#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <CGAL/Exact_spherical_kernel_3.h>
#include <Sphere_intersecter.h>

namespace Ui {
class MainWindow;
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

    // File menu slots
    void openSpheres();
    void saveSpheres();
    void saveSpheresAs();
    void generateSpheres();

private:
    void addNewSphere(const SI::Sphere_handle &sh);
    SI si;
    std::vector<SI::Sphere_handle> sphereList;
    QString openFilename;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
