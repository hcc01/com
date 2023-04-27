#include "manager.h"
#include "ui_manager.h"

Manager::Manager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Manager)
{
    ui->setupUi(this);
}

Manager::~Manager()
{
    delete ui;
}


void Manager::on_testManagerBtn_clicked()
{
    m_testUi.show();
}


void Manager::on_userManagerBtn_clicked()
{
    m_userUi.show();
}

