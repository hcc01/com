#ifndef MANAGER_H
#define MANAGER_H

#include <QMainWindow>
#include"usermanager.h"
#include"testmanager.h"
namespace Ui {
class Manager;
}

class Manager : public QMainWindow
{
    Q_OBJECT

public:
    explicit Manager(QWidget *parent = nullptr);
    ~Manager();

private slots:
    void on_testManagerBtn_clicked();

    void on_userManagerBtn_clicked();

private:
    Ui::Manager *ui;
    UserManager m_userUi;
    TestManager m_testUi;
};

#endif // MANAGER_H
