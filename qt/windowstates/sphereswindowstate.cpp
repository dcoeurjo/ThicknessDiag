#include "sphereswindowstate.h"
#include <fstream>
#include <limits>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QProgressDialog>
#include <QGLViewer/qglviewer.h>
#include <CGAL/Random.h>
#include "../dialogs/sphereformdialog.h"
#include "../dialogs/generatespheresdialog.h"
#include "../dialogs/selectspheredialog.h"
#include "../sphereintersecterproxy.h"
#include "../spherelistwidgetitem.h"

static CGAL::Random randgen;

typedef SphereIntersecter SI;

SpheresWindowState::SpheresWindowState(WindowStateWidget &wsw) :
    WindowStateWithMenu(wsw, tr("Spheres")) {}

SpheresWindowState::~SpheresWindowState()
{ delete sideBarWidget; }

void SpheresWindowState::setup()
{
    // Create menu
    WindowStateWithMenu::setup();

    // Create actions for menu
    QAction *actionNew = new QAction(tr("Create"), this);
    QAction *actionLoad = new QAction(tr("Load"), this);
    QAction *actionSave = new QAction(tr("Save"), this);
    QAction *actionSaveAs = new QAction(tr("Save As"), this);
    QAction *actionGenerate = new QAction(tr("Generate"), this);
    QAction *actionDelete = new QAction(tr("Delete"), this);

    // Set shortcuts
    actionNew->setShortcut(Qt::CTRL + Qt::Key_N);
    actionLoad->setShortcut(Qt::CTRL + Qt::Key_O);
    actionSave->setShortcut(Qt::CTRL + Qt::Key_S);
    actionDelete->setShortcut(Qt::Key_Delete);
    actionSaveAs->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
    actionGenerate->setShortcut(Qt::CTRL + Qt::Key_G);

    // Add actions to menu
    addAction(actionNew);
    addAction(actionLoad);
    addAction(actionSave);
    addAction(actionSaveAs);
    addAction(actionGenerate);
    addAction(actionDelete);

    // Connect menu actions
    QObject::connect(actionNew, SIGNAL(triggered()), this, SLOT(addNewPrompt()));
    QObject::connect(actionLoad, SIGNAL(triggered()), this, SLOT(loadPrompt()));
    QObject::connect(actionSave, SIGNAL(triggered()), this, SLOT(savePrompt()));
    QObject::connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAsPrompt()));
    QObject::connect(actionGenerate, SIGNAL(triggered()), this, SLOT(generatePrompt()));
    QObject::connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteSelected()));

    // Setup sidebar
    sideBarWidget = new QWidget();
    sideBarWidget->setMaximumWidth(150);
    sideBarWidget->hide();
    // vertical layout
    QVBoxLayout *verticalLayout = new QVBoxLayout(sideBarWidget);
    verticalLayout->setSpacing(3);
    verticalLayout->setContentsMargins(1, 1, 1, 1);
    // sphere list widget
    listWidget = new QListWidget(sideBarWidget);
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    listWidget->setSelectionMode(QListWidget::ExtendedSelection);
    verticalLayout->addWidget(listWidget);
    QObject::connect(listWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showCustomContextMenuAt(QPoint)));
    QObject::connect(listWidget, SIGNAL(clicked(QModelIndex)), wsw.viewer(), SLOT(update()));
    // sphere list widget's toggle button
    QPushButton *toggleCheckBtn = new QPushButton(tr("Check/Uncheck all"), sideBarWidget);
    QObject::connect(toggleCheckBtn, SIGNAL(clicked()),
                     this, SLOT(toggleAllCheckState()));
    verticalLayout->addWidget(toggleCheckBtn);
}

void SpheresWindowState::onEnterState()
{
    WindowStateWithMenu::onEnterState();
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(2, -1, 2, -1);
    wsw.setLayout(horizontalLayout);
    horizontalLayout->addWidget(wsw.viewer());
    horizontalLayout->addWidget(sideBarWidget);
    sideBarWidget->show();
    wsw.viewer()->setSceneCenter(qglviewer::Vec(0, 0, 0));
}

void SpheresWindowState::onLeaveState()
{
    WindowStateWithMenu::onLeaveState();
    QHBoxLayout *horizontalLayout = dynamic_cast<QHBoxLayout*>(wsw.layout());
    Q_ASSERT(horizontalLayout != 0);
    horizontalLayout->removeWidget(wsw.viewer());
    horizontalLayout->removeWidget(sideBarWidget);
    horizontalLayout->removeWidget(&wsw);
    delete horizontalLayout;
    sideBarWidget->hide();
}

const SphereView& SpheresWindowState::addNew(const SphereHandle &sh)
{
    // Make view by adding to main window
    const SphereView& sv = wsw.sphereView(sh);

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
    SphereFormDialog sd(&wsw);
    if (sd.exec())
    {
        // Construct sphere and add to intersecter
        Sphere_3 s(Point_3(sd.x, sd.y, sd.z), sd.radius*sd.radius);
        SphereHandle sh = siProxy.addSphere(s);
        if (sh.is_null() == false)
        {
            const SphereView& sv = addNew(sh);
            QString msg = "Sphere " + sv.asString() + "added";
            setStatus(tr(msg.toStdString().c_str()));
            updateDisplay();
        }
        else
        {
            QMessageBox::warning(&wsw, tr("Sphere not added"),
                                 tr("The sphere wasn't added, since it already exists"));
        }
    }
    else
    {
        setStatus(tr("Sphere addition cancelled"));
    }
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
    updateDisplay();
}

void SpheresWindowState::loadPrompt()
{
    // Get file to load spheres from
    QString fileName = QFileDialog::getOpenFileName(&wsw,
            tr("Open spheres"), "", tr("Text files (*.txt)"));
    //if (QFile::exists(fileName) == false) { return; }
    std::ifstream ifs(fileName.toStdString().c_str());

    // Progress bar display
    QProgressDialog pd(&wsw);
    pd.setMinimum(0);
    pd.setCancelButton(0);
    pd.setWindowModality(Qt::WindowModal);
    pd.setContextMenuPolicy(Qt::NoContextMenu);
    pd.setFixedSize(pd.size());

    // Disable menu and window update
    wsw.setUpdatesEnabled(false);

    // Parse file
    std::vector<Sphere_3> spheres;
    std::size_t nblines = std::count(std::istreambuf_iterator<char>(ifs),
                         std::istreambuf_iterator<char>(), '\n');
    if (nblines > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
        std::ostringstream oss;
        oss <<  "Cannot load spheres from " << fileName.toStdString()
             << ": file too large";
        QMessageBox::warning(&wsw, tr("Error loading"), tr(oss.str().c_str()));
        return;
    }
    ifs.seekg(0);
    pd.setMaximum(nblines);
    pd.setLabelText("Parsing spheres from file '" + fileName + "'");
    pd.show();
    spheres.resize(nblines);
    for (std::istream_iterator<Sphere_3> ifs_it(ifs);
         ifs_it != std::istream_iterator<Sphere_3>(); ifs_it++)
    {
        spheres.push_back(*ifs_it);
        pd.setValue(pd.value() + 1);
    }

    // Copy parsed spheres
    std::size_t nb = 0;
    pd.setValue(0);
    pd.setLabelText("Copying spheres and computing intersections");
    for (std::vector<Sphere_3>::const_iterator it = spheres.begin();
         it != spheres.end(); it++)
    {
        SphereHandle sh = siProxy.addSphere(*it);
        if (sh.is_null() == false)
        {
            addNew(sh);
            nb++;
        }
    }

    // Disable menu and window update
    wsw.setUpdatesEnabled(true);

    // Update the UI
    updateDisplay();

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
    QString fileName = QFileDialog::getSaveFileName(&wsw,
            tr("Save spheres"), "", tr("Text files (*.txt)"));
    openFilename = fileName;
    savePrompt();
}

void SpheresWindowState::generatePrompt()
{
    GenerateSpheresDialog gsd(&wsw);
    if (gsd.exec())
    {
        // Progress bar display
        QProgressDialog pd(&wsw);
        pd.setMinimum(0);
        pd.setMaximum(gsd.nb);
        pd.setCancelButton(0);
        pd.setWindowModality(Qt::WindowModal);
        pd.setContextMenuPolicy(Qt::NoContextMenu);
        pd.setFixedSize(pd.size());
        pd.show();

        // Disable menu and window update
        wsw.setUpdatesEnabled(false);

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
            SphereHandle sh = siProxy.addSphere(s);
            if (sh.is_null() == false)
            { addNew(sh); real_nb++; }

            // Update the GUI display
            pd.setValue(nb);
            pd.update();
        }

        // Update the UI
        wsw.setUpdatesEnabled(true);
        updateDisplay();

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
    updateDisplay();

    // Show status message
    std::ostringstream oss;
    oss << "Deleted " << nb << " spheres";
    setStatus(tr(oss.str().c_str()));
}

void SpheresWindowState::draw()
{
    QGLViewer *viewer = wsw.viewer();
    for (int i = 0; i < listWidget->count(); i++)
    {
        // Get item and display if checked
        QListWidgetItem *item = listWidget->item(i);
        if (item->checkState() == Qt::Unchecked)
        { continue; }

        SphereListWidgetItem *sphereItem;
        sphereItem = reinterpret_cast<SphereListWidgetItem*>(item);
        const SphereView &sv = sphereItem->sv();
        sv.draw(viewer);
    }
}

void SpheresWindowState::showCustomContextMenuAt(const QPoint &point)
{
    QMenu contextMenu(tr("Context menu"), &wsw);
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

void SpheresWindowState::updateDisplay()
{
    listWidget->update();
    wsw.viewer()->update();
}
