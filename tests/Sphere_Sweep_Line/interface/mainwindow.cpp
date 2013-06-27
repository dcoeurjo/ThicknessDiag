#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sphereformdialog.h"
#include <fstream>
#include <QFileDialog>
#include <QMessageBox>

typedef MainWindow::SI SI;
typedef MainWindow::Kernel K;
typedef typename K::FT FT;
typedef typename K::Line_3 Line_3;
typedef typename K::Point_3 Point_3;
typedef typename K::Plane_3 Plane_3;
typedef typename K::Vector_3 Vector_3;
typedef typename K::Circle_3 Circle_3;
typedef typename K::Sphere_3 Sphere_3;
typedef typename K::Segment_3 Segment_3;
typedef typename K::Circular_arc_point_3 Circular_arc_point_3;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(680, 605);
    addAction(ui->spheresActionNew);
    addAction(ui->spheresActionDelete);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addNewSphere()
{
    SphereFormDialog sd(this);
    if (sd.exec())
    {
        // Construct sphere and add to intersecter
        Sphere_3 s(Point_3(sd.x, sd.y, sd.z), sd.radius);
        SI::Sphere_handle sh = si.add_sphere(s);
        if (sh.is_null() == false)
        { addNewSphere(sh); }
        else
        { QMessageBox::warning(this, tr("Sphere not added"),
              tr("The sphere wasn't added, since it already exists")); }
    }
}

void MainWindow::addNewSphere(const SI::Sphere_handle &sh)
{
    // Add list to sphere handle vector
    sphereList.push_back(sh);

    // Add sphere to list widget
    using CGAL::to_double;
    std::ostringstream oss;
    const Point_3 &c = sh->center();
    oss << "[" << to_double(c.x())
        << ", " << to_double(c.y())
        << ", " << to_double(c.z())
        << "], " << std::sqrt(to_double(sh->squared_radius()));
    ui->sphereListWidget->addItem(QString(oss.str().c_str()));
    ui->sphereListWidget->update();
}

void MainWindow::openSpheres()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open spheres"), "", "Text files (*.txt)");
    if (QFile::exists(fileName))
    {
        std::ifstream ifs(fileName.toStdString().c_str());
        std::string line;
        while (std::getline(ifs, line))
        {
            std::cout << "Line = " << line << std::endl;
            Sphere_3 s;
            std::istringstream iss(line);
            iss >> s;
            SI::Sphere_handle sh = si.add_sphere(s);
            if (sh.is_null() == false)
            { addNewSphere(sh); }
        }
    }
}

void MainWindow::saveSpheres()
{
    if (openFilename.size() == 0)
    { saveSpheresAs(); }
    else
    {
        // Open file
        std::ofstream ofs(openFilename.toStdString().c_str());

        // Write all
        SI::Sphere_iterator_range sphere_range(si);
        for (SI::Sphere_iterator it = sphere_range.begin();
             it != sphere_range.end(); it++)
        { ofs << **it << std::endl; }
    }
}

void MainWindow::saveSpheresAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save spheres"), "", "Text files (*.txt)");
    openFilename = fileName;
    saveSpheres();
}

void MainWindow::generateSpheres()
{
    std::cout << "generateSpheres" << std::endl; // TODO
}

void MainWindow::deleteSelectedSpheres()
{
    QList<QListWidgetItem *> selectedItems;
    selectedItems = ui->sphereListWidget->selectedItems();
    typedef typename QList<QListWidgetItem *>::const_iterator
            SelectedSpheresIterator;
    for (SelectedSpheresIterator it = selectedItems.begin();
         it != selectedItems.end(); it++)
    {
        QListWidgetItem *item = *it;
        std::vector<SI::Sphere_handle>::iterator sphereIt;
        sphereIt = sphereList.begin() + ui->sphereListWidget->row(item);
        SI::Sphere_handle sh(*sphereIt);
        Q_ASSERT(si.remove_sphere(sh));
        sphereList.erase(sphereIt);
        ui->sphereListWidget->removeItemWidget(item);
        delete item;
    }
    ui->sphereListWidget->update();
}
