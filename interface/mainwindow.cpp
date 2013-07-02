#include <GL/glew.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "spheresstatewidget.h"
#include "sphereintersecterproxy.h"

#include "spheresstatewidget.h"

#define DEFAULT_STATUS_TIMEOUT 3000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    statusTime(DEFAULT_STATUS_TIMEOUT)
{
    ui->setupUi(this);

    // Setup states for main window
    WindowStateWidget * sphereStateWidget = new SpheresStateWidget(this);
    sphereStateWidget->setup();
    setWindowStateWidget(sphereStateWidget);
}

MainWindow::~MainWindow()
{ delete ui; }

void MainWindow::setWindowStateWidget(WindowStateWidget *stateWidget)
{
    // TODO
}

const SphereView& MainWindow::addSphere(const SI::Sphere_handle &sh)
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
    color.setRed(randgen.get_int(0, 255));
    color.setGreen(randgen.get_int(0, 255));
    color.setBlue(randgen.get_int(0, 255));

    // Finally, make sphere
    SphereView sv(sh, color, qglviewer::Vec(x, y, z), radius);

    // Add list to sphere handle vector
    sphereList.push_back(sv);

    // Update the scene so that it includes the sphere
    SI::Bounding_box sceneBbox = SphereIntersecterProxy::access().bbox();
    qglviewer::Vec min(sceneBbox.xmin(), sceneBbox.ymin(),
                       sceneBbox.zmin()),
            max(sceneBbox.xmax(), sceneBbox.ymax(),
                sceneBbox.zmax());
    ui->viewer->setSceneRadius((max - min).norm() / 2.0);

    return sphereList.back();
}

void MainWindow::removeSphere(int index)
{
    sphereList.removeAt(index);
    emit spheresChanged();
}

void MainWindow::drawViewer()
{
    // TODO
}

void MainWindow::showStatus(const QString &status)
{ ui->statusBar->showMessage(status, statusTime); }

QWidget* MainWindow::centralWidget() const
{ return ui->centralWidget; }