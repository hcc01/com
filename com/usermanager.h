#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QMainWindow>

namespace Ui {
class uiManager;
}

class UserManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserManager(QWidget *parent = nullptr);
    ~UserManager();
    void updateList()const;
private slots:
    void on_addButton_clicked();

private:
    Ui::uiManager *ui;
};

#endif // USERMANAGER_H
