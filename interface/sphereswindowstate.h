#ifndef SPHERESSTATEWIDGET_H
#define SPHERESSTATEWIDGET_H

#include <QListWidget>

#include "windowstate.h"
#include <QGLWidget>

class QHBoxLayout;

class SpheresWindowState : public WindowState
{
    Q_OBJECT

public:
    explicit SpheresWindowState(WindowStateWidget &wsw);
    ~SpheresWindowState();

    // Setup override
    void setup();

    // Draw override
    void draw();

protected:
    void onEnterState();
    void onLeaveState();

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
    const SphereView& addNew(const SphereHandle &sh);

    // Layout
    QHBoxLayout *horizontalLayout;

    // Actions
    QAction *actionNew;
    QAction *actionLoad;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionDelete;
    QAction *actionGenerate;

    // Sidebar
    QListWidget *listWidget;

    // Saved check state for list widget
    Qt::CheckState savedCheckState;

    // File opened (default "save" file)
    QString openFilename;
};

#endif // SPHERESSTATEWIDGET_H
