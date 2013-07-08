#include "selectspheredialog.h"
#include "ui_selectspheredialog.h"

#include <QMessageBox>

typedef SphereIntersecter SI;

SelectSphereDialog::SelectSphereDialog(QWidget *parent) :
    QDialog(parent), selectedSphere(0),
    ui(new Ui::SelectSphereDialog)
{
    ui->setupUi(this);
}

SelectSphereDialog::~SelectSphereDialog()
{
    delete ui;
}

void SelectSphereDialog::accept()
{
    if (selectedSphere == 0)
    { QMessageBox::warning(this, "No sphere selected",
          "You must select a sphere"); }
    else
    { QDialog::accept();  }
}

void SelectSphereDialog::addSphere(const SphereView &sv)
{
    QRadioButton *radio = new QRadioButton(sv.asString(), ui->listWidget);
    sphereViewMap[radio] = &sv;
    QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
    ui->listWidget->setItemWidget(item, radio);
    QObject::connect(radio, SIGNAL(clicked(bool)), this, SLOT(radioChanged(bool)));
}

void SelectSphereDialog::radioChanged(bool selected)
{
    if (selected)
    { selectedSphere = sphereViewMap[qobject_cast<QRadioButton*>(sender())]; }
}
