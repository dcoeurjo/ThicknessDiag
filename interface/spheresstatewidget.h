#ifndef SPHERESSTATEWIDGET_H
#define SPHERESSTATEWIDGET_H

#include <QListWidget>

#include <CGAL/Random.h>

#include "windowstatewidget.h"
#include "sphereview.h"

class SpheresStateWidget : public WindowStateWidget
{
    Q_OBJECT

public:
    explicit SpheresStateWidget(MainWindow *w);
    ~SpheresStateWidget();

    void onEnterState();
    void onLeaveState();

    void paintToQGLViewer(QGLViewer *viewer);

protected:
    void setupMenu(QWidget *menubar);
    void setupSidebar(QWidget *sidebar);

private slots:
    void addNewPrompt();
    void loadPrompt();
    void savePrompt();
    void saveAsPrompt();
    void generatePrompt();
    void deleteSelected();
    void toggleAllCheckState();
    void showCustomContextMenuAt(const QPoint &point);

private:
    // Helpers
    const SphereView& addNew(const SphereIntersecter::Sphere_handle &sh);

    // Actions
    QAction *actionNew;
    QAction *actionLoad;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionDelete;
    QAction *actionGenerate;
    QAction *actionStart;

    // Sidebar
    QListWidget *listWidget;

    // Saved check state for list widget
    Qt::CheckState savedCheckState;

    // File opened (default "save" file)
    QString openFilename;

    // Random
    CGAL::Random randgen;
};

#endif // SPHERESSTATEWIDGET_H
