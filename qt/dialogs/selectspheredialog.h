#ifndef SELECTSPHEREDIALOG_H
#define SELECTSPHEREDIALOG_H

#include <QDialog>
#include "../sphereview.h"

namespace Ui {
class SelectSphereDialog;
}

class SelectSphereDialog : public QDialog
{
    Q_OBJECT

public:
     SelectSphereDialog(QWidget *parent = 0);
    ~SelectSphereDialog();

    void addSphere(const SphereView &sv);

    // Selected index
    QList<const SphereView*> selectedSpheres() const;

private slots:
    void accept();

private:
    // Radio button -> position
    QList<const SphereView*> sphereViews;

    // UI
    Ui::SelectSphereDialog *ui;
};

#endif // SELECTSPHEREDIALOG_H
