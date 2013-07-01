#ifndef SELECTSPHEREDIALOG_H
#define SELECTSPHEREDIALOG_H

#include <QHash>
#include <QDialog>
#include <QRadioButton>

#include "sphereintersecter.h"

namespace Ui {
class SelectSphereDialog;
}

class SelectSphereDialog : public QDialog
{
    Q_OBJECT

    typedef SphereIntersecter SI;

public:
    explicit SelectSphereDialog(QWidget *parent = 0);
    ~SelectSphereDialog();

    void addSphereItem(const QString &s);

    // Selected index
    int selectedIndex;

private slots:
    void accept();
    void radioChanged(bool selected);

private:
    // Radio button -> position
    QHash<QRadioButton *, int> positionMap;

    // UI
    Ui::SelectSphereDialog *ui;
};

#endif // SELECTSPHEREDIALOG_H
