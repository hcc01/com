#include "logindialog.h"
#include "ui_logindialog.h"
#include"dbmanager.h"
#include<QMessageBox>
#include"manager.h"
#include"mainwindow.h"
LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowTitle("请登录");
    DBManager dm = DBManager::DM();
    QSqlQuery query(dm.db());

    if(!query.exec("select username from users")){
        qDebug()<<"db_error:"<<query.lastError().text();
    }
    while (query.next()){
        ui->comboBox->addItem(query.value(0).toString());
    }
    if(!query.exec("SELECT username FROM logininfo ORDER BY logindate DESC LIMIT 1")){
        qDebug()<<"查找最近登录时错误:"<<query.lastError().text();
        return;
    }
    QString lastUser;
    if(query.next()){
        lastUser=query.value(0).toString();
        ui->comboBox->setCurrentText(lastUser);
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::accept()
{

}

void LoginDialog::reject()
{
    exit(0);
}

void LoginDialog::on_buttonBox_accepted()
{
    QString username=ui->comboBox->currentText();
    QString password=ui->lineEdit->text();
    DBManager dm = DBManager::DM();
    QSqlQuery query(dm.db());
    if(!query.exec(QString("select * from users where username='%1' and password='%2';").arg(username).arg(password))){
        qDebug()<<"db_error:"<<dm.lastError();
    }
    if(!query.next()){
        QMessageBox::information(nullptr,"error","错误的用户名或密码！");
        return;
    }
    if(username=="admin"){
        Manager* w=new Manager;
        w->show();
    }
    if(!query.exec(QString("insert into logininfo(username,logindate) values('%1',DATETIME('now'));").arg(username))){
        qDebug()<<"保存登录时间错误："<<query.lastError().text();
    }

    MainWindow* w=new MainWindow;
    w->setWindowTitle("南方海洋 - "+username);
    w->show();
    w->setUser(username);
    QDialog::accept();
}

