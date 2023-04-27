#include "widget.h"
#include "ui_widget.h"
#include<QMessageBox>
#include<QExcel.h>
#include<QFile>
#include<QDateTime>
#include<QDir>
#include<QDebug>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
//    disconnect(ui->testButton, SIGNAL(clicked()), this, SLOT(on_testButton_clicked()));
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
       {
           QSerialPort serial;
           serial.setPort(info);
           if(serial.open(QIODevice::ReadWrite))
           {
               ui->PortBox->addItem(serial.portName());
               serial.close();
           }
       }
       //设置波特率下拉菜单默认显示第0项
       ui->BaudBox->setCurrentIndex(0);
       m_timer=new QTimer(this);

       connect(m_timer,&QTimer::timeout,this,&Widget::ReadData);
}

Widget::~Widget()
{
    delete ui;
}

QString Widget::getMass(const QString &txt) const
{
    return txt;
}

void Widget::bufferData()
{
    m_timer->start(50);
    m_buffer.append(serial->readAll());
}


void Widget::on_OpenSerialButton_clicked()
{
    if(ui->OpenSerialButton->text() == tr("打开串口"))
        {
            serial = new QSerialPort;
            //设置串口名
            serial->setPortName(ui->PortBox->currentText());
            //打卡串口
            if(!serial->open(QIODevice::ReadWrite)){
                QMessageBox::information(this,"","打开串口失败。");
                delete serial;
                serial=nullptr;
                return;
            }
            //设置波特率
            serial->setBaudRate(ui->BaudBox->currentText().toInt());
            //设置数据位数
            switch (ui->BitBox->currentIndex())
            {
            case 8:
                serial->setDataBits(QSerialPort::Data8);
            case 7:
                serial->setDataBits(QSerialPort::Data7);
            case 6:
                serial->setDataBits(QSerialPort::Data6);
            case 5:
                serial->setDataBits(QSerialPort::Data5);
                break;
            default:
                break;
            }
            //设置校验位
            switch (ui->ParityBox->currentIndex())
            {
            case 0:
                serial->setParity(QSerialPort::NoParity);
                break;
            default:
                break;
            }
            //设置停止位
            switch (ui->BitBox->currentIndex())
            {
            case 1:
                serial->setStopBits(QSerialPort::OneStop);
                break;
            case 2:
                serial->setStopBits(QSerialPort::TwoStop);
            default:
                break;
            }
            //设置流控制
            serial->setFlowControl(QSerialPort::NoFlowControl);

            //关闭设置菜单使能
            ui->PortBox->setEnabled(false);
            ui->BaudBox->setEnabled(false);
            ui->BitBox->setEnabled(false);
            ui->ParityBox->setEnabled(false);
            ui->StopBox->setEnabled(false);
            ui->OpenSerialButton->setText(tr("关闭串口"));

            //连接信号槽
            connect(serial,SIGNAL(readyRead()),this,SLOT(bufferData()));
        }
        else
        {
            //关闭串口
            serial->clear();
            serial->close();
            serial->deleteLater();

            //恢复设置使能
            ui->PortBox->setEnabled(true);
            ui->BaudBox->setEnabled(true);
            ui->BitBox->setEnabled(true);
            ui->ParityBox->setEnabled(true);
            ui->StopBox->setEnabled(true);
            ui->OpenSerialButton->setText(tr("打开串口"));
        }
}
void Widget::ReadData()
{

     m_timer->stop();
    if(!m_buffer.isEmpty())
    {
        QString str =tr(m_buffer);
        QFile file("./log.txt");

        // Open the file in write mode
        if (!file.open(QIODevice::Append | QIODevice::Text))
            return;

        // Create a text stream object
        QTextStream out(&file);

        // Write some text to the file
        out<<QString("收到数据：size=%1,data=%2。todouble: %3").arg(m_buffer.length()).arg(QString(m_buffer.toHex())).arg(m_buffer.toDouble());
        out <<QDateTime::currentDateTime().toString()<<"    str= "<<str;

        // Close the file
        file.close();
        int n1=str.indexOf("WT:");
        if(n1<0){
            QMessageBox::information(this,"error","数据格式错误。");
            return;
        }
        str=str.mid(n1+3);
        str=str.left(str.indexOf("g"));
        QMessageBox::information(this,"",str);

    }
    m_buffer.clear();
}

