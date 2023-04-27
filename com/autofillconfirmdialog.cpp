#include "autofillconfirmdialog.h"
#include "ui_autofillconfirmdialog.h"

AutoFillConfirmDialog::AutoFillConfirmDialog(const QStringList &numbers, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoFillConfirmDialog)
{
    ui->setupUi(this);
    ui->listWidget->addItems(numbers);
}

AutoFillConfirmDialog::~AutoFillConfirmDialog()
{
    delete ui;
}

int AutoFillConfirmDialog::getIndex(const QStringList &numbers)
{
    AutoFillConfirmDialog dialog(numbers);
    int result = dialog.exec();
    if (result == QDialog::Accepted) {
        return dialog.ui->listWidget->currentIndex().row();
    } else {
        return -1;
    }
}


void AutoFillConfirmDialog::on_buttonBox_accepted()
{

}

