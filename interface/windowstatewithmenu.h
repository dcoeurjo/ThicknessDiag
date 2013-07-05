#ifndef WINDOWSTATEWITHMENU_H
#define WINDOWSTATEWITHMENU_H

#include "windowstate.h"

class QMenu;
class QAction;

class WindowStateWithMenu : public WindowState
{
public:
    explicit WindowStateWithMenu(WindowStateWidget &wsw, const QString &n);
    virtual ~WindowStateWithMenu();

    virtual void setup();

protected:
    // Add an action to the menu, s
    void addAction(QAction *action);

    // Reimplemented enter/leave state bundle
    virtual void onEnterState();
    virtual void onLeaveState();

private:
    QMenu *menu;
};

#endif // WINDOWSTATEWITHMENU_H
