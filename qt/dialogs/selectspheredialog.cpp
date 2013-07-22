#include "selectspheredialog.h"
#include "ui_selectspheredialog.h"
#include <QMessageBox>

SelectSphereDialog::SelectSphereDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::SelectSphereDialog())
{ ui->setupUi(this); }

SelectSphereDialog::~SelectSphereDialog()
{ delete ui; }

void SelectSphereDialog::addSphere(const SphereView &sv)
{
    QListWidgetItem *item = new QListWidgetItem(sv.asString(), ui->listWidget);
    item->setCheckState(Qt::Unchecked);
    ui->listWidget->addItem(item);
    sphereViews.push_back(&sv);
}

QList<const SphereView*> SelectSphereDialog::selectedSpheres() const
{
    QList<const SphereView*> spheres;
    foreach(QListWidgetItem *item, ui->listWidget->findItems("*", Qt::MatchWildcard))
    { if (item->checkState() == Qt::Checked)
      { spheres.push_back(sphereViews[ui->listWidget->row(item)]); } }
    return spheres;
}

void SelectSphereDialog::accept()
{
    foreach(QListWidgetItem *item, ui->listWidget->findItems("*", Qt::MatchWildcard))
    { if (item->checkState() == Qt::Checked)
      { QDialog::accept(); return; } }
    QMessageBox::warning(this, tr("No sphere selected"),
                         tr("You must select at least one sphere"));
}
