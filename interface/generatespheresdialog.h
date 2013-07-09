#ifndef GENERATESPHERESDIALOG_H
#define GENERATESPHERESDIALOG_H

#include <QDialog>

namespace Ui {
class GenerateSpheresDialog;
}

class GenerateSpheresDialog : public QDialog
{
    Q_OBJECT

public:
     GenerateSpheresDialog(QWidget *parent = 0);
    ~GenerateSpheresDialog();

    double center[3];
    double amplitude[3];

    double radius;
    std::size_t nb;

private slots:
    void accept();

private:
    Ui::GenerateSpheresDialog *ui;
};

#endif // GENERATESPHERESDIALOG_H
