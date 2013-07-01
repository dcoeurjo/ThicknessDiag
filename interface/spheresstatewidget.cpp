#include "spheresstatewidget.h"

#include <fstream>

#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>

#include "sphereformdialog.h"
#include "generatespheresdialog.h"
#include "selectspheredialog.h"
#include "sphereintersecterproxy.h"

typedef SphereIntersecter SI;
typedef SphereIntersecterKernel K;

SpheresStateWidget::SpheresStateWidget(MainWindow * w) :
    WindowStateWidget(w)
{
    QMenu *menu = WindowStateWidget::menu();
    menu->setTitle("Spheres");

    actionNew = new QAction(tr("Create"), this);
    actionLoad = new QAction(tr("Load"), this);
    actionSave = new QAction(tr("Save"), this);
    actionSaveAs = new QAction(tr("Save As"), this);
    actionDelete = new QAction(tr("Delete"), this);
    actionGenerate = new QAction(tr("Generate"), this);
    actionStart = new QAction(tr("Start mode"), this);

    menu->addAction(actionNew);
    menu->addAction(actionLoad);
    menu->addAction(actionSave);
    menu->addAction(actionSaveAs);
    menu->addAction(actionGenerate);
    menu->addAction(actionDelete);
    menu->addAction(actionStart);

    connect(actionNew, SIGNAL(triggered()), this, SLOT(addNewPrompt()));
    connect(actionLoad, SIGNAL(triggered()), this, SLOT(loadPrompt()));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(savePrompt()));
    connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAsPrompt()));
    connect(actionGenerate, SIGNAL(triggered()), this, SLOT(generateNewPrompt()));
    connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteSelected()));
    connect(actionStart, SIGNAL(triggered()), this, SIGNAL(enterRequested()));

    listWidget = new QListWidget(sidebar());
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(listWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showCustomContextMenuAt(QPoint)));
}

SpheresStateWidget::~SpheresStateWidget()
{
}

void SpheresStateWidget::onEnterState()
{
    actionNew->setEnabled(true);
    actionDelete->setEnabled(true);
    actionGenerate->setEnabled(true);
    actionStart->setEnabled(false);
}

void SpheresStateWidget::onLeaveState()
{
    actionNew->setEnabled(false);
    actionDelete->setEnabled(false);
    actionGenerate->setEnabled(false);
    actionStart->setEnabled(true);
}

const SphereView& SpheresStateWidget::addNew(const SI::Sphere_handle &sh)
{
    // Make view by adding to main window
    const SphereView& sv = mainWindow()->addSphere(sh);

    // Add to list
    QListWidgetItem *item;
    item = new QListWidgetItem(sv.asString(), listWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    listWidget->addItem(item);

    return sv;
}

void SpheresStateWidget::addNewPrompt()
{
    SphereFormDialog sd(this);
    if (sd.exec())
    {
        // Construct sphere and add to intersecter
        Sphere_3 s(Point_3(sd.x, sd.y, sd.z), sd.radius*sd.radius);
        SI::Sphere_handle sh = si().add_sphere(s);
        if (sh.is_null() == false)
        { const SphereView& sv = addNew(sh);
            setStatus("Sphere " + sv.asString() + "added"); }
        else
        { QMessageBox::warning(this, tr("Sphere not added"),
              tr("The sphere wasn't added, since it already exists")); }
    }
    else
    { setStatus("Sphere addition cancelled"); }
}

void SpheresStateWidget::toggleAllCheckState()
{
    // Toggle state
    savedCheckState = (savedCheckState == Qt::Unchecked)
            ? Qt::Checked : Qt::Unchecked;
    for (int i = 0; i < listWidget->count(); i++)
    {
        QListWidgetItem *item = listWidget->item(i);
        item->setCheckState(savedCheckState);
    }

    // Update UI
    listWidget->update();
}

void SpheresStateWidget::loadPrompt()
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
        SI::Sphere_handle sh = si().add_sphere(*it);
        if (sh.is_null() == false)
        { addNew(sh);
          nb++; }
    }

    // Show status message
    std::ostringstream oss;
    oss << "Loaded " << nb << " spheres";
    setStatus(QString(oss.str().c_str()));
}

void SpheresStateWidget::savePrompt()
{
    if (openFilename.size() == 0)
    { saveAsPrompt(); }
    else
    {
        // Open file
        std::ofstream ofs(openFilename.toStdString().c_str());

        // Write all
        SI::Sphere_iterator_range sphere_range(si());
        for (SI::Sphere_iterator it = sphere_range.begin();
             it != sphere_range.end(); it++)
        { ofs << **it << std::endl; }

        // Show status message
        setStatus("Saved spheres to " + openFilename);
    }
}

void SpheresStateWidget::saveAsPrompt()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save spheres"), "", "Text files (*.txt)");
    openFilename = fileName;
    savePrompt();
}

void SpheresStateWidget::generatePrompt()
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
            double x = randgen.get_double(gsd.center[0] - gsd.amplitude[0],
                            gsd.center[0] + gsd.amplitude[0]),
                    y = randgen.get_double(gsd.center[1] - gsd.amplitude[1],
                            gsd.center[1] + gsd.amplitude[1]),
                    z = randgen.get_double(gsd.center[2] - gsd.amplitude[2],
                            gsd.center[2] + gsd.amplitude[2]);

            // Generate radius (not nil);
            double radius = 0;
            while (radius == 0)
            { radius = randgen.get_double(0, gsd.radius); }

            // Construct sphere and add to intersecter
            Sphere_3 s(Point_3(x, y, z), radius*radius);
            SI::Sphere_handle sh = si().add_sphere(s);
            if (sh.is_null() == false)
            { addNew(sh); real_nb++; }

            // Update the GUI display
            pd.setValue(nb);
            pd.update();
        }

        // Update the UI
        setUpdatesEnabled(true);
        listWidget->update();

        // Show status message
        std::ostringstream oss;
        oss << "Generated " << real_nb << " spheres";
        setStatus(QString(oss.str().c_str()));
    }
}

void SpheresStateWidget::deleteSelected()
{
    // Get selected (stop if none)
    QList<QListWidgetItem *> selectedItems;
    selectedItems = listWidget->selectedItems();
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
        int index = listWidget->row(item);
        mainWindow()->removeSphere(index);
        listWidget->removeItemWidget(item);
        delete item;
        nb++;
    }

    // Update UI
    listWidget->update();

    // Show status message
    std::ostringstream oss;
    oss << "Deleted " << nb << " spheres";
    setStatus(QString(oss.str().c_str()));
}

void SpheresStateWidget::paintToQGLViewer(QGLViewer *viewer)
{
    for (int i = 0; i < listWidget->count(); i++)
    {
        // Get item and display if checked
        QListWidgetItem *item = listWidget->item(i);
        if (item->checkState() == Qt::Unchecked)
        { continue; }

        const SphereView &sv = mainWindow()->sphere(listWidget->row(item));

        // Actually draw the sphere
        glPushMatrix();
            viewer->qglColor(sv.color);
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

void SpheresStateWidget::showCustomContextMenuAt(const QPoint &point)
{
    QMenu contextMenu(tr("Context menu"), this);
    QListWidgetItem *item = listWidget->currentItem();
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
    contextMenu.exec(listWidget->mapToGlobal(point));
}