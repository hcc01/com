#ifndef OPENFILEDIALOG_H
#define OPENFILEDIALOG_H

#include <QDialog>
#include<QSqlQuery>
namespace Ui {
class OpenFileDialog;
}

class OpenFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenFileDialog(const QString& user,QWidget *parent = nullptr);
    ~OpenFileDialog();
    static QList<QVariant> getSelectedData(const QString& user);
private slots:
    void on_buttonBox_accepted();

private:
    Ui::OpenFileDialog *ui;
    QList<QVariant> m_selectedData;
    QString m_user;
};

#endif // OPENFILEDIALOG_H
