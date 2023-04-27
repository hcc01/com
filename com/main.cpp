
#include <QApplication>
#include<QFile>
#include"widget.h"
#include"logindialog.h"
#include<QIcon>
#include"QExcel.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/logo/logo.ico"));
    QFile qssFile("./style.qss");
    qssFile.open(QFile::ReadOnly);
    if (qssFile.isOpen()) {
        QString qssStyle = QLatin1String(qssFile.readAll());
        a.setStyleSheet(qssStyle);
    }

    LoginDialog l;
    l.exec();
//    Widget w;
//    w.show();
    return a.exec();
}
