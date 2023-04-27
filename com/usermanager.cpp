#include "usermanager.h"
#include "ui_uimanager.h"
#include"dbmanager.h"
#include<QMessageBox>
UserManager::UserManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::uiManager)
{
    ui->setupUi(this);
    updateList();
}

UserManager::~UserManager()
{
    delete ui;
}

void UserManager::updateList() const
{
    ui->listWidget->clear();
    DBManager dm=DBManager::DM();
    bool ok;
    QSqlQuery query=dm.query("select username from users where username != 'admin';",ok);
    if(!ok){
        qDebug()<<dm.lastError();
        return;
    }
    while(query.next()){
        ui->listWidget->addItem(query.value(0).toString());
    }
}

void UserManager::on_addButton_clicked()
{
    QString name=ui->nameEdit->text();
    QString password=ui->passwordEdit->text();
    QString password2=ui->passwordEdit2->text();
    if(name.isEmpty()){
        QMessageBox::information(this,"error","请输入用户名");
        return;
    }
    if(password!=password2){
        QMessageBox::information(this,"error","再次密码不一致");
        return;
    }
    DBManager dm=DBManager::DM();
    bool ok;
    dm.query(QString("insert into users(username,password) values('%1','%2');").arg(name).arg(password),ok);
    if(!ok){
        QMessageBox::information(this,"error","增加人员失败，错误代码："+dm.lastError());
        qDebug()<<dm.lastError();
    }
    updateList();
}

