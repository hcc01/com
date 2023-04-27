#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QSerialPortInfo>
#include<QMessageBox>
#include<QFile>
#include<QDateTime>
#include<QDebug>
#include<QTimer>
#include"QExcel.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_serialOpen(false)
{
    ui->setupUi(this);
    m_weigh=new WeighUI(this);
    serial = new QSerialPort(this);
    //连接信号槽
    connect(serial,SIGNAL(readyRead()),this,SLOT(buffData()));
    m_timer=new QTimer;
    connect(m_timer,&QTimer::timeout,this,&MainWindow::ReadData);
    setWindowIcon(QIcon(":/logo/logo.ico"));
}

MainWindow::~MainWindow()
{

    closeSerial();
    delete ui;
    delete m_timer;
}

void MainWindow::enable(bool b)
{
    if(b){
        ui->startButton->setEnabled(true);
        return;
    }
    ui->startButton->setEnabled(false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(m_weigh->excelOpened) EXCEL.Quit();
}

void MainWindow::on_startButton_clicked()
{

     if(!ui->testList->currentIndex().isValid()){
        QMessageBox::information(this,"","请选择一个分析项目!");
        return;
     }
    openSerial();

    enable(false);
    m_weigh->reset();
    m_weigh->setTestItem(ui->testList->currentItem()->text(),m_user);
    m_weigh->show();
}

void MainWindow::ReadData()
{
     m_timer->stop();
    if(!m_buffer.isEmpty())
    {
        QString str =tr(m_buffer);
//        QFile file("./log.txt");

//        // Open the file in write mode
//        if (!file.open(QIODevice::Append | QIODevice::Text))
//            return;

//        // Create a text stream object
//        QTextStream out(&file);

//        // Write some text to the file
//        out<<QString("收到数据：size=%1,data=%2。todouble: %3").arg(m_buffer.length()).arg(QString(m_buffer.toHex())).arg(m_buffer.toDouble());
//        out <<QDateTime::currentDateTime().toString()<<"    str= "<<str;

//        // Close the file
//        file.close();
        int n1=str.indexOf("WT:");
        if(n1<0){
            QMessageBox::information(this,"error","数据格式错误。");
            return;
        }
        str=str.mid(n1+3);
        str=str.left(str.indexOf("g"));
        m_weigh->readData(str);

    }
    m_buffer.clear();
}

void MainWindow::openSerial()
{
    if(m_serialOpen) return;
    //设置串口名
    serial->setPortName(QSerialPortInfo::availablePorts().at(0).portName());
    //打卡串口
    if(!serial->open(QIODevice::ReadWrite)){
        QMessageBox::information(this,"","打开串口失败。");
        delete serial;
        serial=nullptr;
        return;
    }
    //设置波特率
    serial->setBaudRate(9600);
    //设置数据位数

    serial->setDataBits(QSerialPort::Data8);

    //设置校验位

    serial->setParity(QSerialPort::NoParity);

    //设置停止位

    serial->setStopBits(QSerialPort::OneStop);

    //设置流控制
    serial->setFlowControl(QSerialPort::NoFlowControl);

    m_serialOpen=true;

}

void MainWindow::closeSerial()
{
    serial->clear();
    serial->close();
    m_serialOpen=false;
}

void MainWindow::buffData()
{
    m_timer->start(50);
    m_buffer.append(serial->readAll());
}

