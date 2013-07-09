/********************************************************************************
** Form generated from reading UI file 'generatespheresdialog.ui'
**
** Created: Fri Jul 5 08:57:41 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GENERATESPHERESDIALOG_H
#define UI_GENERATESPHERESDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GenerateSpheresDialog
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_da;
    QLabel *label_da;
    QGridLayout *gridLayout_da;
    QLabel *label_da_x;
    QLabel *label_da_y;
    QLabel *label_da_z;
    QDoubleSpinBox *doubleSpinBox_da_x;
    QDoubleSpinBox *doubleSpinBox_da_y;
    QDoubleSpinBox *doubleSpinBox_da_z;
    QVBoxLayout *verticalLayout_c;
    QLabel *label_c;
    QGridLayout *gridLayout_c;
    QLabel *label_c_x;
    QLabel *label_c_y;
    QLabel *label_c_z;
    QDoubleSpinBox *doubleSpinBox_c_x;
    QDoubleSpinBox *doubleSpinBox_c_y;
    QDoubleSpinBox *doubleSpinBox_c_z;
    QHBoxLayout *horizontalLayout_r;
    QLabel *label_r;
    QDoubleSpinBox *doubleSpinBox_r;
    QHBoxLayout *horizontalLayout_nb;
    QLabel *label_nb;
    QSpinBox *spinBox_nb;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *GenerateSpheresDialog)
    {
        if (GenerateSpheresDialog->objectName().isEmpty())
            GenerateSpheresDialog->setObjectName(QString::fromUtf8("GenerateSpheresDialog"));
        GenerateSpheresDialog->resize(453, 388);
        layoutWidget = new QWidget(GenerateSpheresDialog);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 433, 365));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(30);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout_da = new QVBoxLayout();
        verticalLayout_da->setObjectName(QString::fromUtf8("verticalLayout_da"));
        label_da = new QLabel(layoutWidget);
        label_da->setObjectName(QString::fromUtf8("label_da"));

        verticalLayout_da->addWidget(label_da);

        gridLayout_da = new QGridLayout();
        gridLayout_da->setObjectName(QString::fromUtf8("gridLayout_da"));
        gridLayout_da->setHorizontalSpacing(20);
        gridLayout_da->setVerticalSpacing(8);
        label_da_x = new QLabel(layoutWidget);
        label_da_x->setObjectName(QString::fromUtf8("label_da_x"));
        label_da_x->setAlignment(Qt::AlignCenter);

        gridLayout_da->addWidget(label_da_x, 0, 0, 1, 1);

        label_da_y = new QLabel(layoutWidget);
        label_da_y->setObjectName(QString::fromUtf8("label_da_y"));
        label_da_y->setAlignment(Qt::AlignCenter);

        gridLayout_da->addWidget(label_da_y, 0, 1, 1, 1);

        label_da_z = new QLabel(layoutWidget);
        label_da_z->setObjectName(QString::fromUtf8("label_da_z"));
        label_da_z->setAlignment(Qt::AlignCenter);

        gridLayout_da->addWidget(label_da_z, 0, 2, 1, 1);

        doubleSpinBox_da_x = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_da_x->setObjectName(QString::fromUtf8("doubleSpinBox_da_x"));
        doubleSpinBox_da_x->setMinimumSize(QSize(0, 27));
        doubleSpinBox_da_x->setMinimum(-1e+09);
        doubleSpinBox_da_x->setMaximum(1e+09);
        doubleSpinBox_da_x->setValue(0);

        gridLayout_da->addWidget(doubleSpinBox_da_x, 1, 0, 1, 1);

        doubleSpinBox_da_y = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_da_y->setObjectName(QString::fromUtf8("doubleSpinBox_da_y"));
        doubleSpinBox_da_y->setMinimumSize(QSize(0, 27));
        doubleSpinBox_da_y->setMinimum(-1e+09);
        doubleSpinBox_da_y->setMaximum(1e+09);
        doubleSpinBox_da_y->setValue(0);

        gridLayout_da->addWidget(doubleSpinBox_da_y, 1, 1, 1, 1);

        doubleSpinBox_da_z = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_da_z->setObjectName(QString::fromUtf8("doubleSpinBox_da_z"));
        doubleSpinBox_da_z->setMinimumSize(QSize(0, 27));
        doubleSpinBox_da_z->setMinimum(-1e+09);
        doubleSpinBox_da_z->setMaximum(1e+09);
        doubleSpinBox_da_z->setValue(0);

        gridLayout_da->addWidget(doubleSpinBox_da_z, 1, 2, 1, 1);


        verticalLayout_da->addLayout(gridLayout_da);


        verticalLayout->addLayout(verticalLayout_da);

        verticalLayout_c = new QVBoxLayout();
        verticalLayout_c->setObjectName(QString::fromUtf8("verticalLayout_c"));
        label_c = new QLabel(layoutWidget);
        label_c->setObjectName(QString::fromUtf8("label_c"));

        verticalLayout_c->addWidget(label_c);

        gridLayout_c = new QGridLayout();
        gridLayout_c->setObjectName(QString::fromUtf8("gridLayout_c"));
        gridLayout_c->setHorizontalSpacing(20);
        gridLayout_c->setVerticalSpacing(8);
        label_c_x = new QLabel(layoutWidget);
        label_c_x->setObjectName(QString::fromUtf8("label_c_x"));
        label_c_x->setAlignment(Qt::AlignCenter);

        gridLayout_c->addWidget(label_c_x, 0, 0, 1, 1);

        label_c_y = new QLabel(layoutWidget);
        label_c_y->setObjectName(QString::fromUtf8("label_c_y"));
        label_c_y->setAlignment(Qt::AlignCenter);

        gridLayout_c->addWidget(label_c_y, 0, 1, 1, 1);

        label_c_z = new QLabel(layoutWidget);
        label_c_z->setObjectName(QString::fromUtf8("label_c_z"));
        label_c_z->setAlignment(Qt::AlignCenter);

        gridLayout_c->addWidget(label_c_z, 0, 2, 1, 1);

        doubleSpinBox_c_x = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_c_x->setObjectName(QString::fromUtf8("doubleSpinBox_c_x"));
        doubleSpinBox_c_x->setMinimumSize(QSize(0, 27));
        doubleSpinBox_c_x->setMinimum(-1e+09);
        doubleSpinBox_c_x->setMaximum(1e+09);
        doubleSpinBox_c_x->setValue(0);

        gridLayout_c->addWidget(doubleSpinBox_c_x, 1, 0, 1, 1);

        doubleSpinBox_c_y = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_c_y->setObjectName(QString::fromUtf8("doubleSpinBox_c_y"));
        doubleSpinBox_c_y->setMinimumSize(QSize(0, 27));
        doubleSpinBox_c_y->setMinimum(-1e+09);
        doubleSpinBox_c_y->setMaximum(1e+09);
        doubleSpinBox_c_y->setValue(0);

        gridLayout_c->addWidget(doubleSpinBox_c_y, 1, 1, 1, 1);

        doubleSpinBox_c_z = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_c_z->setObjectName(QString::fromUtf8("doubleSpinBox_c_z"));
        doubleSpinBox_c_z->setMinimumSize(QSize(0, 27));
        doubleSpinBox_c_z->setMinimum(-1e+09);
        doubleSpinBox_c_z->setMaximum(1e+09);
        doubleSpinBox_c_z->setValue(0);

        gridLayout_c->addWidget(doubleSpinBox_c_z, 1, 2, 1, 1);


        verticalLayout_c->addLayout(gridLayout_c);


        verticalLayout->addLayout(verticalLayout_c);

        horizontalLayout_r = new QHBoxLayout();
        horizontalLayout_r->setObjectName(QString::fromUtf8("horizontalLayout_r"));
        label_r = new QLabel(layoutWidget);
        label_r->setObjectName(QString::fromUtf8("label_r"));

        horizontalLayout_r->addWidget(label_r);

        doubleSpinBox_r = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_r->setObjectName(QString::fromUtf8("doubleSpinBox_r"));
        doubleSpinBox_r->setMinimum(0);
        doubleSpinBox_r->setMaximum(1e+09);
        doubleSpinBox_r->setValue(0);

        horizontalLayout_r->addWidget(doubleSpinBox_r);


        verticalLayout->addLayout(horizontalLayout_r);

        horizontalLayout_nb = new QHBoxLayout();
        horizontalLayout_nb->setObjectName(QString::fromUtf8("horizontalLayout_nb"));
        label_nb = new QLabel(layoutWidget);
        label_nb->setObjectName(QString::fromUtf8("label_nb"));

        horizontalLayout_nb->addWidget(label_nb);

        spinBox_nb = new QSpinBox(layoutWidget);
        spinBox_nb->setObjectName(QString::fromUtf8("spinBox_nb"));
        spinBox_nb->setMaximum(999);

        horizontalLayout_nb->addWidget(spinBox_nb);


        verticalLayout->addLayout(horizontalLayout_nb);

        buttonBox = new QDialogButtonBox(layoutWidget);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(GenerateSpheresDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), GenerateSpheresDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), GenerateSpheresDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(GenerateSpheresDialog);
    } // setupUi

    void retranslateUi(QDialog *GenerateSpheresDialog)
    {
        GenerateSpheresDialog->setWindowTitle(QApplication::translate("GenerateSpheresDialog", "Generate a sphere", 0, QApplication::UnicodeUTF8));
        label_da->setText(QApplication::translate("GenerateSpheresDialog", "Distribution amplitudes", 0, QApplication::UnicodeUTF8));
        label_da_x->setText(QApplication::translate("GenerateSpheresDialog", "X", 0, QApplication::UnicodeUTF8));
        label_da_y->setText(QApplication::translate("GenerateSpheresDialog", "Y", 0, QApplication::UnicodeUTF8));
        label_da_z->setText(QApplication::translate("GenerateSpheresDialog", "Z", 0, QApplication::UnicodeUTF8));
        label_c->setText(QApplication::translate("GenerateSpheresDialog", "Center of distribution", 0, QApplication::UnicodeUTF8));
        label_c_x->setText(QApplication::translate("GenerateSpheresDialog", "X", 0, QApplication::UnicodeUTF8));
        label_c_y->setText(QApplication::translate("GenerateSpheresDialog", "Y", 0, QApplication::UnicodeUTF8));
        label_c_z->setText(QApplication::translate("GenerateSpheresDialog", "Z", 0, QApplication::UnicodeUTF8));
        label_r->setText(QApplication::translate("GenerateSpheresDialog", "Radius amplitude", 0, QApplication::UnicodeUTF8));
        label_nb->setText(QApplication::translate("GenerateSpheresDialog", "Number of spheres", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GenerateSpheresDialog: public Ui_GenerateSpheresDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENERATESPHERESDIALOG_H
