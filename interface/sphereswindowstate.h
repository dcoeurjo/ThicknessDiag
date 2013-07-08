#ifndef SPHERESSTATEWIDGET_H
#define SPHERESSTATEWIDGET_H

#include <QListWidget>

#include "windowstatewithmenu.h"
#include <QGLWidget>

class SpheresWindowState : public WindowStateWithMenu
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
    void updateDisplay();

    // Sidebar
    QWidget *sideBarWidget;
    QListWidget *listWidget;

    // Saved check state for list widget
    Qt::CheckState savedCheckState;

    // File opened (default "save" file)
    QString openFilename;
};

#endif // SPHERESSTATEWIDGET_H
