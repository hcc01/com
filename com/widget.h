#ifndef WIDGET_H
#define WIDGET_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QWidget>
#include<QTimer>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QString getMass(const QString&)const;

private slots:
    void bufferData();
    void on_OpenSerialButton_clicked();
    void ReadData();
    void on_testButton_clicked();

private:
    Ui::Widget *ui;
    QSerialPort* serial;    
    QByteArray m_buffer;

    QTimer *m_timer;
};
#endif // WIDGET_H
