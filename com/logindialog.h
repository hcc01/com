#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    void accept()override;
    void reject()override;
private slots:
    void on_buttonBox_accepted();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
