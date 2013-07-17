#ifndef SPHEREFORMDIALOG_H
#define SPHEREFORMDIALOG_H

#include <QDialog>

namespace Ui {
class SphereFormDialog;
}

class SphereFormDialog : public QDialog
{
    Q_OBJECT

public:
     SphereFormDialog(QWidget *parent = 0);
    ~SphereFormDialog();
    double x, y, z;
    double radius;

private slots:
    void accept();

private:
    Ui::SphereFormDialog *ui;
};

#endif // SPHEREFORMDIALOG_H
