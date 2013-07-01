#ifndef WINDOWSTATEWIDGET_H
#define WINDOWSTATEWIDGET_H

#include <QMenu>
#include <QMenuBar>
#include <QWidget>

#include <QGLViewer/qglviewer.h>

#include "mainwindow.h"
#include "sphereintersecter.h"

class WindowStateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WindowStateWidget(MainWindow * w);
    virtual ~WindowStateWidget();

    virtual void onEnterState() = 0;
    virtual void onLeaveState() = 0;

    virtual void paintToQGLViewer(QGLViewer *viewer) = 0;

signals:
    void enterRequested();
    void stateEntered();
    void stateLeft();

protected:
    MainWindow* mainWindow() const
    { return mw; }

    QMenu* menu() const; // TODO
    QWidget* sidebar() const; // TODO

    SphereIntersecter& si() const;

    void setStatus(const QString &msg)
    { mw->showStatus(msg); }

private:
    MainWindow *mw;
    QMenu *menuWidget;
    QWidget *sidebarWidget;
};

#endif // WINDOWSTATEWIDGET_H
