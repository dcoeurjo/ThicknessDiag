#ifndef SPHERESSTATEWIDGET_H
#define SPHERESSTATEWIDGET_H

#include <QListWidget>

#include "windowstate.h"
#include <QGLWidget>

class SpheresWindowState : public WindowState
{
    Q_OBJECT

public:
    explicit SpheresWindowState(WindowStateWidget &wsw);
    ~SpheresWindowState();

    void onEnterState();
    void onLeaveState();

    void paintToQGLViewer(QGLViewer *viewer);

protected:
    void setupMenu(QMenu *menu);
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
    void addNew(const SphereHandle &sh);

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
};

#endif // SPHERESSTATEWIDGET_H
