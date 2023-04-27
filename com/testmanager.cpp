#include "testmanager.h"
#include "ui_testmanager.h"

TestManager::TestManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TestManager)
{
    ui->setupUi(this);
}

TestManager::~TestManager()
{
    delete ui;
}
