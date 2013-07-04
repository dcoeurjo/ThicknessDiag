#include "sphereswindowstate.h"

#include <fstream>

#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>

#include <QGLViewer/qglviewer.h>

#include <CGAL/Random.h>

#include "sphereformdialog.h"
#include "generatespheresdialog.h"
#include "selectspheredialog.h"
#include "sphereintersecterproxy.h"

#include "spherelistwidgetitem.h"

typedef SphereIntersecter SI;

SpheresWindowState::SpheresWindowState(WindowStateWidget &wsw) :
    WindowState(wsw)
{
    setName(tr("Spheres"));
}

SpheresWindowState::~SpheresWindowState() {}

void SpheresWindowState::setupWidget(QWidget *widget)
{
    // Create menu
    QMenu *menu = mw().menuBar()->addMenu(name());

    // Create actions for menu
    actionNew = new QAction(tr("Create"), this);
    actionLoad = new QAction(tr("Load"), this);
    actionSave = new QAction(tr("Save"), this);
    actionSaveAs = new QAction(tr("Save As"), this);
    actionDelete = new QAction(tr("Delete"), this);
    actionGenerate = new QAction(tr("Generate"), this);

    // Add actions to menu
    menu->addAction(actionNew);
    menu->addAction(actionLoad);
    menu->addAction(actionSave);
    menu->addAction(actionSaveAs);
    menu->addAction(actionGenerate);
    menu->addAction(actionDelete);

    // Connect menu actions
    QObject::connect(actionNew, SIGNAL(triggered()), this, SLOT(addNewPrompt()));
    QObject::connect(actionLoad, SIGNAL(triggered()), this, SLOT(loadPrompt()));
    QObject::connect(actionSave, SIGNAL(triggered()), this, SLOT(savePrompt()));
    QObject::connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAsPrompt()));
    QObject::connect(actionGenerate, SIGNAL(triggered()), this, SLOT(generatePrompt()));
    QObject::connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteSelected()));

    // Setup sidebar
    listWidget = new QListWidget(widget);
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(listWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showCustomContextMenuAt(QPoint)));
}

void SpheresWindowState::onEnterState()
{
    actionNew->setEnabled(true);
    actionDelete->setEnabled(true);
    actionGenerate->setEnabled(true);
}

void SpheresWindowState::onLeaveState()
{
    actionNew->setEnabled(false);
    actionDelete->setEnabled(false);
    actionGenerate->setEnabled(false);
}

const SphereView& SpheresWindowState::addNew(const SphereHandle &sh)
{
    // Make view by adding to main window
    const SphereView& sv = wsw().sphereView(sh);

    // Add to list
    QListWidgetItem *item;
    item = new SphereListWidgetItem(sv, listWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    listWidget->addItem(item);

    return sv;
}

void SpheresWindowState::addNewPrompt()
{
    SphereFormDialog sd(&wsw());
    if (sd.exec())
    {
        // Construct sphere and add to intersecter
        Sphere_3 s(Point_3(sd.x, sd.y, sd.z), sd.radius*sd.radius);
        SphereHandle sh = siProxy.addSphere(s);
        if (sh.is_null() == false)
        { const SphereView& sv = addNew(sh);
            QString msg = "Sphere " + sv.asString() + "added";
            setStatus(tr(msg.toStdString().c_str())); }
        else
        { QMessageBox::warning(&wsw(), tr("Sphere not added"),
              tr("The sphere wasn't added, since it already exists")); }
    }
    else
    { setStatus(tr("Sphere addition cancelled")); }
}

void SpheresWindowState::toggleAllCheckState()
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

void SpheresWindowState::loadPrompt()
{
    // Get file to load spheres from
    QString fileName = QFileDialog::getOpenFileName(&wsw(),
            tr("Open spheres"), "", tr("Text files (*.txt)"));
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
        SphereHandle sh = siProxy.addSphere(*it);
        if (sh.is_null() == false)
        { addNew(sh);
          nb++; }
    }

    // Show status message
    std::ostringstream oss;
    oss << "Loaded " << nb << " spheres";
    setStatus(QString(oss.str().c_str()));
}

void SpheresWindowState::savePrompt()
{
    if (openFilename.size() == 0)
    { saveAsPrompt(); }
    else
    {
        // Open file
        std::ofstream ofs(openFilename.toStdString().c_str());

        // Write all
        SI::Sphere_iterator_range sphere_range(siProxy.directAccess());
        for (SI::Sphere_iterator it = sphere_range.begin();
             it != sphere_range.end(); it++)
        { ofs << **it << std::endl; }

        // Show status message
        setStatus(tr("Saved spheres to ") + openFilename);
    }
}

void SpheresWindowState::saveAsPrompt()
{
    QString fileName = QFileDialog::getSaveFileName(&wsw(),
            tr("Save spheres"), "", tr("Text files (*.txt)"));
    openFilename = fileName;
    savePrompt();
}

void SpheresWindowState::generatePrompt()
{
    GenerateSpheresDialog gsd(&wsw());
    if (gsd.exec())
    {
        // Progress bar display
        QProgressDialog pd(&wsw());
        pd.setMinimum(0);
        pd.setMaximum(gsd.nb);
        pd.setCancelButton(0);
        pd.setWindowModality(Qt::WindowModal);
        pd.setContextMenuPolicy(Qt::NoContextMenu);
        pd.setFixedSize(pd.size());
        pd.show();

        // Disable menu and window update
        wsw().setUpdatesEnabled(false);

        // Generate spheres
        CGAL::Random randgen;
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
            SphereHandle sh = siProxy.addSphere(s);
            if (sh.is_null() == false)
            { addNew(sh); real_nb++; }

            // Update the GUI display
            pd.setValue(nb);
            pd.update();
        }

        // Update the UI
        wsw().setUpdatesEnabled(true);
        listWidget->update();

        // Show status message
        std::ostringstream oss;
        oss << "Generated " << real_nb << " spheres";
        setStatus(tr(oss.str().c_str()));
    }
}

void SpheresWindowState::deleteSelected()
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
        SphereListWidgetItem *item;
        item = dynamic_cast<SphereListWidgetItem*>(*it);
        siProxy.removeSphere(item->sv().handle);
        listWidget->removeItemWidget(item);
        delete item;
        nb++;
    }

    // Update UI
    listWidget->update();

    // Show status message
    std::ostringstream oss;
    oss << "Deleted " << nb << " spheres";
    setStatus(tr(oss.str().c_str()));
}

void SpheresWindowState::drawToViewer(QGLViewer *viewer)
{
    for (int i = 0; i < listWidget->count(); i++)
    {
        // Get item and display if checked
        QListWidgetItem *item = listWidget->item(i);
        if (item->checkState() == Qt::Unchecked)
        { continue; }

        SphereListWidgetItem *sphereItem;
        sphereItem = reinterpret_cast<SphereListWidgetItem*>(item);
        const SphereView &sv = sphereItem->sv();

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

void SpheresWindowState::showCustomContextMenuAt(const QPoint &point)
{
    QMenu contextMenu(tr("Context menu"), &wsw());
    QListWidgetItem *item = listWidget->currentItem();
    if (item && item->isSelected())
    {
        // "delete" action"
        QAction *deleteAction = new QAction(tr("Delete"), &contextMenu);
        QObject::connect(deleteAction, SIGNAL(triggered()),
                         this, SLOT(deleteSelected()));
        contextMenu.addAction(deleteAction);
    }
    else
    {
        // "new sphere" action
        QAction *newSphereAction = new QAction(tr("Add new sphere"), &contextMenu);
        QObject::connect(newSphereAction, SIGNAL(triggered()),
                         this, SLOT(addNewPrompt()));
        contextMenu.addAction(newSphereAction);

        // "generate spheres" action
        QAction *genAction = new QAction(tr("Generate spheres"), &contextMenu);
        QObject::connect(genAction, SIGNAL(triggered()),
                         this, SLOT(generatePrompt()));
        contextMenu.addAction(genAction);

    }
    contextMenu.exec(listWidget->mapToGlobal(point));
}