#ifndef TESTMANAGER_H
#define TESTMANAGER_H

#include <QMainWindow>

namespace Ui {
class TestManager;
}

class TestManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestManager(QWidget *parent = nullptr);
    ~TestManager();

private:
    Ui::TestManager *ui;
};

#endif // TESTMANAGER_H
