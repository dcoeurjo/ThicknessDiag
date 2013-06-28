#include "generatespheresdialog.h"
#include "ui_generatespheresdialog.h"

#include <QMessageBox>

#define DEFAULT_X_AMPLITUDE 20
#define DEFAULT_Y_AMPLITUDE 20
#define DEFAULT_Z_AMPLITUDE 20
#define DEFAULT_R_AMPLITUDE 20
#define DEFAULT_NB_SPHERES 10

GenerateSpheresDialog::GenerateSpheresDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GenerateSpheresDialog)
{
    ui->setupUi(this);
    setFixedSize(453, 388);

    // Default values
    ui->doubleSpinBox_da_x->setValue(DEFAULT_X_AMPLITUDE);
    ui->doubleSpinBox_da_y->setValue(DEFAULT_Y_AMPLITUDE);
    ui->doubleSpinBox_da_z->setValue(DEFAULT_Z_AMPLITUDE);
    ui->doubleSpinBox_r->setValue(DEFAULT_R_AMPLITUDE);
    ui->spinBox_nb->setValue(DEFAULT_NB_SPHERES);
}

GenerateSpheresDialog::~GenerateSpheresDialog()
{
    delete ui;
}

void GenerateSpheresDialog::accept()
{
    // Convert center coordinates
    center[0] = ui->doubleSpinBox_c_x->text().toDouble();
    center[1] = ui->doubleSpinBox_c_y->text().toDouble();
    center[2] = ui->doubleSpinBox_c_z->text().toDouble();

    // Convert sphere coordinate distributions
    amplitude[0] = ui->doubleSpinBox_da_x->text().toDouble();
    amplitude[1] = ui->doubleSpinBox_da_y->text().toDouble();
    amplitude[2] = ui->doubleSpinBox_da_z->text().toDouble();

    // Convert sphere radius amplitude
    radius = ui->doubleSpinBox_r->text().toDouble();

    // Convert nb spheres to generate
    nb = ui->spinBox_nb->text().toLong();

    // Check and accept
    if (radius == 0)
    { QMessageBox::warning(this, tr("Radius must not be nil"),
          tr("Sphere radius cannot be zero !")); }
    else
    { QDialog::accept(); }
}