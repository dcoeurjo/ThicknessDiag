/********************************************************************************
** Form generated from reading UI file 'sphereformdialog.ui'
**
** Created: Fri Jul 5 08:57:41 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPHEREFORMDIALOG_H
#define UI_SPHEREFORMDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SphereFormDialog
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label_x;
    QLabel *label_y;
    QLabel *label_z;
    QLabel *label_radius;
    QDoubleSpinBox *doubleSpinBox_radius;
    QDoubleSpinBox *doubleSpinBox_z;
    QDoubleSpinBox *doubleSpinBox_y;
    QDoubleSpinBox *doubleSpinBox_x;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SphereFormDialog)
    {
        if (SphereFormDialog->objectName().isEmpty())
            SphereFormDialog->setObjectName(QString::fromUtf8("SphereFormDialog"));
        SphereFormDialog->resize(220, 195);
        layoutWidget = new QWidget(SphereFormDialog);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 201, 177));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setHorizontalSpacing(19);
        formLayout->setVerticalSpacing(8);
        label_x = new QLabel(layoutWidget);
        label_x->setObjectName(QString::fromUtf8("label_x"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_x);

        label_y = new QLabel(layoutWidget);
        label_y->setObjectName(QString::fromUtf8("label_y"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_y);

        label_z = new QLabel(layoutWidget);
        label_z->setObjectName(QString::fromUtf8("label_z"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_z);

        label_radius = new QLabel(layoutWidget);
        label_radius->setObjectName(QString::fromUtf8("label_radius"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_radius);

        doubleSpinBox_radius = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_radius->setObjectName(QString::fromUtf8("doubleSpinBox_radius"));
        doubleSpinBox_radius->setDecimals(2);
        doubleSpinBox_radius->setMinimum(0);
        doubleSpinBox_radius->setMaximum(1e+09);

        formLayout->setWidget(3, QFormLayout::FieldRole, doubleSpinBox_radius);

        doubleSpinBox_z = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_z->setObjectName(QString::fromUtf8("doubleSpinBox_z"));
        doubleSpinBox_z->setDecimals(2);
        doubleSpinBox_z->setMinimum(-1e+09);
        doubleSpinBox_z->setMaximum(1e+09);

        formLayout->setWidget(2, QFormLayout::FieldRole, doubleSpinBox_z);

        doubleSpinBox_y = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_y->setObjectName(QString::fromUtf8("doubleSpinBox_y"));
        doubleSpinBox_y->setDecimals(2);
        doubleSpinBox_y->setMinimum(-1e+09);
        doubleSpinBox_y->setMaximum(1e+09);

        formLayout->setWidget(1, QFormLayout::FieldRole, doubleSpinBox_y);

        doubleSpinBox_x = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_x->setObjectName(QString::fromUtf8("doubleSpinBox_x"));
        doubleSpinBox_x->setDecimals(2);
        doubleSpinBox_x->setMinimum(-1e+09);
        doubleSpinBox_x->setMaximum(1e+09);

        formLayout->setWidget(0, QFormLayout::FieldRole, doubleSpinBox_x);


        verticalLayout->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(layoutWidget);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(SphereFormDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SphereFormDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SphereFormDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SphereFormDialog);
    } // setupUi

    void retranslateUi(QDialog *SphereFormDialog)
    {
        SphereFormDialog->setWindowTitle(QApplication::translate("SphereFormDialog", "Create a sphere", 0, QApplication::UnicodeUTF8));
        label_x->setText(QApplication::translate("SphereFormDialog", "X", 0, QApplication::UnicodeUTF8));
        label_y->setText(QApplication::translate("SphereFormDialog", "Y", 0, QApplication::UnicodeUTF8));
        label_z->setText(QApplication::translate("SphereFormDialog", "Z", 0, QApplication::UnicodeUTF8));
        label_radius->setText(QApplication::translate("SphereFormDialog", "Radius", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SphereFormDialog: public Ui_SphereFormDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPHEREFORMDIALOG_H
