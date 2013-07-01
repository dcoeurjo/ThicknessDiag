#include "selectspheredialog.h"
#include "ui_selectspheredialog.h"

#include <QMessageBox>

typedef SphereIntersecter SI;

SelectSphereDialog::SelectSphereDialog(QWidget *parent) :
    QDialog(parent), selectedIndex(-1),
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
    if (selectedIndex == -1)
    { QMessageBox::warning(this, "No sphere selected",
          "You must select a sphere"); }
    else
    { QDialog::accept();  }
}

void SelectSphereDialog::addSphereItem(const QString &s)
{
    QRadioButton *radio = new QRadioButton(s, ui->listWidget);
    positionMap[radio] = ui->listWidget->count();
    QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
    ui->listWidget->setItemWidget(item, radio);
    QObject::connect(radio, SIGNAL(clicked(bool)), this, SLOT(radioChanged(bool)));
}

void SelectSphereDialog::radioChanged(bool selected)
{
    if (selected)
    { selectedIndex = positionMap[qobject_cast<QRadioButton *>(sender())]; }
}
