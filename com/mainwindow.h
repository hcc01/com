#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"weighui.h"
#include<QSerialPort>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setUser(const QString&name){
        m_user=name;
    }
    void enable(bool );
    void closeEvent(QCloseEvent *event)override;
private slots:
    void on_startButton_clicked();
    void ReadData();
    void openSerial();
    void closeSerial();
    void buffData();
private:
    Ui::MainWindow *ui;
    WeighUI* m_weigh;
    QSerialPort* serial;
    QString m_user;
    QTimer* m_timer;
    QByteArray m_buffer;
    bool m_serialOpen;
};

#endif // MAINWINDOW_H
