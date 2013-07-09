/********************************************************************************
** Form generated from reading UI file 'selectspheredialog.ui'
**
** Created: Fri Jul 5 08:57:42 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTSPHEREDIALOG_H
#define UI_SELECTSPHEREDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SelectSphereDialog
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QListWidget *listWidget;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SelectSphereDialog)
    {
        if (SelectSphereDialog->objectName().isEmpty())
            SelectSphereDialog->setObjectName(QString::fromUtf8("SelectSphereDialog"));
        SelectSphereDialog->resize(233, 381);
        gridLayout = new QGridLayout(SelectSphereDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        listWidget = new QListWidget(SelectSphereDialog);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        verticalLayout->addWidget(listWidget);

        buttonBox = new QDialogButtonBox(SelectSphereDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(SelectSphereDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SelectSphereDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SelectSphereDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SelectSphereDialog);
    } // setupUi

    void retranslateUi(QDialog *SelectSphereDialog)
    {
        SelectSphereDialog->setWindowTitle(QApplication::translate("SelectSphereDialog", "Select a sphere", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SelectSphereDialog: public Ui_SelectSphereDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTSPHEREDIALOG_H
