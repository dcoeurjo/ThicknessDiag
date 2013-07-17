#ifndef SELECTSPHEREDIALOG_H
#define SELECTSPHEREDIALOG_H

#include <QHash>
#include <QList>
#include <QDialog>
#include <QRadioButton>
#include "../sphereview.h"

namespace Ui {
class SelectSphereDialog;
}

class SelectSphereDialog : public QDialog
{
    Q_OBJECT

    typedef SphereIntersecter SI;

public:
     SelectSphereDialog(QWidget *parent = 0);
    ~SelectSphereDialog();

    void addSphere(const SphereView &sv);

    // Selected index
    const SphereView* selectedSphere;

private slots:
    void accept();
    void radioChanged(bool selected);

private:
    // Radio button -> position
    QHash<QRadioButton*, const SphereView*> sphereViewMap;

    // UI
    Ui::SelectSphereDialog *ui;
};

#endif // SELECTSPHEREDIALOG_H
