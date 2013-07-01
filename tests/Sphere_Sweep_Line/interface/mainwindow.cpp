#include <GL/glew.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sphereformdialog.h"
#include "generatespheresdialog.h"

#include <fstream>

#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>

#include <CGAL/Random.h>

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
    ui(new Ui::MainWindow),
    random(new CGAL::Random())
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete random;
    delete ui;
}

void MainWindow::addNewSphere()
{
    SphereFormDialog sd(this);
    if (sd.exec())
    {
        // Construct sphere and add to intersecter
        Sphere_3 s(Point_3(sd.x, sd.y, sd.z), sd.radius*sd.radius);
        SI::Sphere_handle sh = si.add_sphere(s);
        if (sh.is_null() == false)
        { addNewSphere(sh); }
        else
        { QMessageBox::warning(this, tr("Sphere not added"),
              tr("The sphere wasn't added, since it already exists")); }
    }
}

void MainWindow::openSpheres()
{
    // Get file to load spheres from
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open spheres"), "", "Text files (*.txt)");
    if (QFile::exists(fileName) == false)
    { return; }

    // Parse file
    std::ifstream ifs(fileName.toStdString().c_str());
    std::vector<Sphere_3> spheres;
    std::copy(std::istream_iterator<Sphere_3>(ifs),
        std::istream_iterator<Sphere_3>(),
        std::back_inserter(spheres));

    // Copy parsed spheres
    for (std::vector<Sphere_3>::const_iterator it = spheres.begin();
         it != spheres.end(); it++)
    {
        SI::Sphere_handle sh = si.add_sphere(*it);
        if (sh.is_null() == false)
        { addNewSphere(sh); }
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
    GenerateSpheresDialog gsd(this);
    if (gsd.exec())
    {
        // Progress bar display
        QProgressDialog pd(this);
        pd.setMinimum(0);
        pd.setMaximum(gsd.nb);
        pd.setCancelButton(0);
        pd.setWindowModality(Qt::WindowModal);
        pd.setContextMenuPolicy(Qt::NoContextMenu);
        pd.setFixedSize(pd.size());
        pd.show();

        // Disable menu and window update
        setUpdatesEnabled(false);

        // Generate spheres
        for (std::size_t nb = 0; nb < gsd.nb; nb++)
        {
            // Generate random numbers
            double x = random->get_double(gsd.center[0] - gsd.amplitude[0],
                            gsd.center[0] + gsd.amplitude[0]),
                    y = random->get_double(gsd.center[1] - gsd.amplitude[1],
                            gsd.center[1] + gsd.amplitude[1]),
                    z = random->get_double(gsd.center[2] - gsd.amplitude[2],
                            gsd.center[2] + gsd.amplitude[2]);

            // Generate radius (not nil);
            double radius = 0;
            while (radius == 0)
            { radius = random->get_double(0, gsd.radius); }

            // Construct sphere and add to intersecter
            Sphere_3 s(Point_3(x, y, z), radius*radius);
            SI::Sphere_handle sh = si.add_sphere(s);
            if (sh.is_null() == false)
            { addNewSphere(sh, false); }

            // Update the GUI display
            pd.setValue(nb);
            pd.update();
        }

        // Update the UI
        setUpdatesEnabled(true);
        ui->sphereListWidget->update();
        ui->viewer->update();
    }
}

void MainWindow::deleteSelectedSpheres()
{
    // Get selected (stop if none)
    QList<QListWidgetItem *> selectedItems;
    selectedItems = ui->sphereListWidget->selectedItems();
    if (selectedItems.empty())
    { return; }

    // Delete selected
    typedef typename QList<QListWidgetItem *>::const_iterator
            SelectedSpheresIterator;
    for (SelectedSpheresIterator it = selectedItems.begin();
         it != selectedItems.end(); it++)
    {
        QListWidgetItem *item = *it;
        SphereList::iterator sphereIt;
        sphereIt = sphereViewList.begin() + ui->sphereListWidget->row(item);
        SI::Sphere_handle sh = sphereIt->handle;
        Q_ASSERT(si.remove_sphere(sh)); // TODO remove as a range (better speed)
        sphereViewList.erase(sphereIt);
        ui->sphereListWidget->removeItemWidget(item);
        delete item;
    }
    ui->sphereListWidget->update();
    ui->viewer->update();
}

void MainWindow::drawSpheres()
{
    for (int i = 0; i < ui->sphereListWidget->count(); i++)
    {
        // Get item and display if checked
        QListWidgetItem *item = ui->sphereListWidget->item(i);
        if (item->checkState() == Qt::Unchecked)
        { continue; }

        // Get corresponding sphere view
        SphereList::iterator sphereIt;
        sphereIt = sphereViewList.begin() + ui->sphereListWidget->row(item);
        const Sphere_view &sv = *sphereIt;

        // Actually draw the sphere
        glPushMatrix();
            ui->viewer->qglColor(sv.color);
            glMultMatrixd(sv.frame.matrix());
            GLUquadricObj * sphere = gluNewQuadric();
            gluQuadricDrawStyle(sphere, GLU_SILHOUETTE);
            gluQuadricNormals(sphere, GLU_SMOOTH);
            gluQuadricOrientation(sphere, GLU_OUTSIDE);
            gluSphere(sphere, sv.radius, 20, 20);
            gluDeleteQuadric(sphere);
        glPopMatrix();
    }
}

void MainWindow::toggleAllSpheresCheckState()
{
    savedCheckState = (savedCheckState == Qt::Unchecked)
            ? Qt::Checked : Qt::Unchecked;
    for (int i = 0; i < ui->sphereListWidget->count(); i++)
    {
        QListWidgetItem *item = ui->sphereListWidget->item(i);
        item->setCheckState(savedCheckState);
    }
    ui->sphereListWidget->update();
    ui->viewer->update();
}

void MainWindow::showSpheresMenuAt(const QPoint &point)
{
    QMenu contextMenu(tr("Context menu"), this);
    QListWidgetItem *item = ui->sphereListWidget->currentItem();
    if (item && item->isSelected())
    {
        // "delete" action"
        QAction *deleteAction = new QAction(tr("Delete"), this);
        QObject::connect(deleteAction, SIGNAL(triggered()),
                         this, SLOT(deleteSelectedSpheres()));
        contextMenu.addAction(deleteAction);
    }
    else
    {
        // "new sphere" action
        QAction *newSphereAction = new QAction(tr("Add new sphere"), this);
        QObject::connect(newSphereAction, SIGNAL(triggered()),
                         this, SLOT(addNewSphere()));
        contextMenu.addAction(newSphereAction);

        // "generate spheres" action
        QAction *genAction = new QAction(tr("Generate spheres"), this);
        QObject::connect(genAction, SIGNAL(triggered()),
                         this, SLOT(generateSpheres()));
        contextMenu.addAction(genAction);

    }
    contextMenu.exec(ui->sphereListWidget->mapToGlobal(point));
}

void MainWindow::addNewSphere(const SI::Sphere_handle &sh, bool updateUI)
{
    // Compute approximate data
    using CGAL::to_double;
    const Point_3 &c = sh->center();
    double radius = std::sqrt(to_double(sh->squared_radius())),
            x = to_double(c.x()),
            y = to_double(c.y()),
            z = to_double(c.z());

    // Make view text
    std::ostringstream oss;
    oss << "[" << x << ", " << y
        << ", " << z << "], " << radius;
    QString text(oss.str().c_str());

    // Make sphere color
    QColor color;
    color.setRed(random->get_int(0, 255));
    color.setGreen(random->get_int(0, 255));
    color.setBlue(random->get_int(0, 255));

    // Finally, make sphere
    Sphere_view sv = { sh, color };
    sv.radius = radius;
    sv.frame.setPosition(x, y, z);

    // Add list to sphere handle vector
    sphereViewList.push_back(sv);

    // Add sphere view to list widget (as text)
    QListWidgetItem *item;
    item = new QListWidgetItem(text, ui->sphereListWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    ui->sphereListWidget->addItem(item);

    // Update the scene so that it includes the sphere
    SI::Bounding_box sceneBbox = si.bbox();
    qglviewer::Vec min(sceneBbox.xmin(), sceneBbox.ymin(),
                       sceneBbox.zmin()),
            max(sceneBbox.xmax(), sceneBbox.ymax(),
                sceneBbox.zmax());
    ui->viewer->setSceneRadius((max - min).norm() / 2.0);

    // Update views if requested
    if (updateUI)
    {
        ui->sphereListWidget->update();
        ui->viewer->update();
    }
}
