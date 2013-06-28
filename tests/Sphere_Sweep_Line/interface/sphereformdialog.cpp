#include "sphereformdialog.h"
#include "ui_sphereformdialog.h"

#include <QMessageBox>

SphereFormDialog::SphereFormDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SphereFormDialog)
{
    ui->setupUi(this);
    setFixedSize(220, 195);
}

SphereFormDialog::~SphereFormDialog()
{
    delete ui;
}

void SphereFormDialog::accept()
{
    x = ui->doubleSpinBox_x->text().toDouble();
    y = ui->doubleSpinBox_y->text().toDouble();
    z = ui->doubleSpinBox_z->text().toDouble();
    radius = ui->doubleSpinBox_radius->text().toDouble();
    if (radius > 0)
    { QDialog::accept(); }
    else
    { QMessageBox::warning(this, tr("Radius must not be nil"),
          tr("Sphere radius cannot be zero !")); }
}
