#include <GL/glew.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sphereformdialog.h"
#include "generatespheresdialog.h"
#include "selectspheredialog.h"

#include <fstream>

#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>

#include <CGAL/Random.h>

typedef SphereIntersecter SI;
typedef SphereIntersecterKernel K;

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
    enterSpheresMode();
}

MainWindow::~MainWindow()
{
    delete random;
    delete ui;
}

MainWindow::SphereView::SphereView():
    handle(), color(),
    frame(), radius()
{
}

MainWindow::SphereView::SphereView(const SI::Sphere_handle &sh,
        const QColor &color, const qglviewer::Vec &pos, double radius):
    handle(sh), color(color),
    frame(), radius(radius)
{
    frame.setPosition(pos);
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
        { addNewSphere(sh);
          showStatusMessage("Sphere " + sphereString(s) + "added"); }
        else
        { QMessageBox::warning(this, tr("Sphere not added"),
              tr("The sphere wasn't added, since it already exists")); }
    }
    else
    {
        showStatusMessage("Sphere addition cancelled");
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
    std::size_t nb = 0;
    for (std::vector<Sphere_3>::const_iterator it = spheres.begin();
         it != spheres.end(); it++)
    {
        SI::Sphere_handle sh = si.add_sphere(*it);
        if (sh.is_null() == false)
        { addNewSphere(sh);
          nb++; }
    }

    // Show status message
    std::ostringstream oss;
    oss << "Loaded " << nb << " spheres";
    showStatusMessage(QString(oss.str().c_str()));
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

        // Show status message
        showStatusMessage("Saved spheres to " + openFilename);
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
        std::size_t real_nb = 0;
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
            { addNewSphere(sh, false);
              real_nb++; }

            // Update the GUI display
            pd.setValue(nb);
            pd.update();
        }

        // Update the UI
        setUpdatesEnabled(true);
        ui->sphereListWidget->update();
        ui->viewer->update();

        // Show status message
        std::ostringstream oss;
        oss << "Generated " << real_nb << " spheres";
        showStatusMessage(QString(oss.str().c_str()));
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
    std::size_t nb = 0;
    typedef typename QList<QListWidgetItem *>::const_iterator
            SelectedSpheresIterator;
    for (SelectedSpheresIterator it = selectedItems.begin();
         it != selectedItems.end(); it++)
    {
        QListWidgetItem *item = *it;
        SphereList::iterator sphereIt;
        sphereIt = sphereList.begin() + ui->sphereListWidget->row(item);
        SI::Sphere_handle sh = sphereIt->handle;
        Q_ASSERT(si.remove_sphere(sh)); // TODO remove as a range (better speed)
        sphereList.erase(sphereIt);
        ui->sphereListWidget->removeItemWidget(item);
        delete item;
        nb++;
    }

    // Update UI
    ui->sphereListWidget->update();
    ui->viewer->update();

    // Show status message
    std::ostringstream oss;
    oss << "Deleted " << nb << " spheres";
    showStatusMessage(QString(oss.str().c_str()));
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
        sphereIt = sphereList.begin() + ui->sphereListWidget->row(item);
        const SphereView &sv = *sphereIt;

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
    // Toggle state
    savedCheckState = (savedCheckState == Qt::Unchecked)
            ? Qt::Checked : Qt::Unchecked;
    for (int i = 0; i < ui->sphereListWidget->count(); i++)
    {
        QListWidgetItem *item = ui->sphereListWidget->item(i);
        item->setCheckState(savedCheckState);
    }

    // Update UI
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

QString MainWindow::sphereString(const Sphere_3 &s)
{
    // Compute approximate data
    using CGAL::to_double;
    const Point_3 &c = s.center();
    double radius = std::sqrt(to_double(s.squared_radius())),
            x = to_double(c.x()),
            y = to_double(c.y()),
            z = to_double(c.z());

    // Make view text
    std::ostringstream oss;
    oss << "[" << x << ", " << y
        << ", " << z << "], " << radius;
    return QString(oss.str().c_str());
}

QString MainWindow::sphereString(const SI::Sphere_handle &sh)
{
    return sphereString(*sh);
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

    // Make sphere color
    QColor color;
    color.setRed(random->get_int(0, 255));
    color.setGreen(random->get_int(0, 255));
    color.setBlue(random->get_int(0, 255));

    // Make sphere text
    QString text(sphereString(sh));

    // Finally, make sphere
    SphereView sv(sh, color, qglviewer::Vec(x, y, z), radius);

    // Add list to sphere handle vector
    sphereList.push_back(sv);

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

void MainWindow::buildEventQueue()
{
    // Build dialog
    SelectSphereDialog ssd(this);
    for (int i = 0; i < ui->sphereListWidget->count(); i++)
    {
        QListWidgetItem *item = ui->sphereListWidget->item(i);
        ssd.addSphereItem(item->text());
    }

    // Execute and handle
    if (ssd.exec())
    {
        //SphereList::iterator sphereIt;
        //sphereIt = sphereList.begin() + ssd.selectedIndex;
        QMessageBox::information(this, "Selected sphere",
            ui->sphereListWidget->item(ssd.selectedIndex)->text());
    }
}

void MainWindow::showStatusMessage(const QString &msg)
{
    ui->statusBar->showMessage(msg, 3000);
    ui->statusBar->update();
}

void MainWindow::enterSpheresMode()
{
    changeMode(SPHERES);

    // Deactivate event queue mode
    { QList<QAction *>  ls = ui->menuEvent_queue->actions();
    for (int i = 0; i < ls.count(); i++)
    { ls.at(i)->setEnabled(ls.at(i) == ui->eqActionStartMode); } }

    // Activate spheres mode
    { QList<QAction *>  ls = ui->menuSpheres->actions();
    for (int i = 0; i < ls.count(); i++)
    { ls.at(i)->setEnabled(ls.at(i) != ui->spheresActionStartMode); } }
}

void MainWindow::enterEventQueueMode()
{
    changeMode(EVENT_QUEUE);

    // Deactivate event queue mode
    { QList<QAction *>  ls = ui->menuSpheres->actions();
    for (int i = 0; i < ls.count(); i++)
    { ls.at(i)->setEnabled(ls.at(i) == ui->spheresActionStartMode); } }

    // Activate spheres mode
    { QList<QAction *>  ls = ui->menuEvent_queue->actions();
    for (int i = 0; i < ls.count(); i++)
    { ls.at(i)->setEnabled(ls.at(i) != ui->eqActionStartMode); } }
}

void MainWindow::changeMode(const Mode &m)
{
    emit modeEntered(m);
    mode = m;
}

void MainWindow::drawViewer()
{
    switch (mode)
    {
    case SPHERES:
        drawSpheres();
        break;

    case EVENT_QUEUE:
        break;

    default:
        break;
    }
}