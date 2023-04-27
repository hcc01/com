#ifndef AUTOFILLCONFIRMDIALOG_H
#define AUTOFILLCONFIRMDIALOG_H

#include <QDialog>

namespace Ui {
class AutoFillConfirmDialog;
}

class AutoFillConfirmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoFillConfirmDialog(const QStringList&,QWidget *parent = nullptr);
    ~AutoFillConfirmDialog();
    static int getIndex(const QStringList&);
private slots:
    void on_buttonBox_accepted();
signals:

private:
    Ui::AutoFillConfirmDialog *ui;
};

#endif // AUTOFILLCONFIRMDIALOG_H
