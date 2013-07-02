#ifndef WINDOWSTATEWIDGET_H
#define WINDOWSTATEWIDGET_H

#include <QMenu>
#include <QMenuBar>
#include <QWidget>

#include <QGLViewer/qglviewer.h>

#include "mainwindow.h"
#include "sphereintersecter.h"

class SphereIntersecterProxy;

class WindowStateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WindowStateWidget(MainWindow * w);
    virtual ~WindowStateWidget();

    void setup();

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

    // Setup callbacks
    virtual void setupMenu(QMenu *menu) = 0;
    virtual void setupSidebar(QWidget *sidebar) = 0;

    // Configuration callbacks
    virtual QString menuTitle() const
    { return ""; }

    void setStatus(const QString &msg)
    { mw->showStatus(msg); }

    SphereIntersecterProxy *si_proxy;

private:
    MainWindow *mw;
};

#endif // WINDOWSTATEWIDGET_H
