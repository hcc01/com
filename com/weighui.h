#ifndef WEIGHUI_H
#define WEIGHUI_H

#include <QMainWindow>
#include<QDateTime>
#include<QLabel>
namespace Ui {
class WeighUI;
}

class WeighUI : public QMainWindow
{
    Q_OBJECT

public:
    bool excelOpened;
    explicit WeighUI(QWidget *parent = nullptr);
    ~WeighUI();
    void closeEvent(QCloseEvent *event)override;
    void reset();
    void setTestItem(const QString&item, const QString &userName);
    void readData(const QString& data);
    void print(bool selectedOnly=false);
//    void save(const QString&fileName);
    double round(double value, int decimals) {
        double factor = pow(10, decimals);
        value *= factor;
        int intPart = (int)value;
        double fracPart = value - intPart;
        if (fracPart < 0.5) {
            return intPart / factor;
        } else if (fracPart > 0.5) {
            return (intPart + 1) / factor;
        } else {
            int lastDigit = (int)(fracPart * 10) % 10;
            if (lastDigit == 0) {
                return intPart / factor;
            } else if (intPart % 2 == 0) {
                return intPart / factor;
            } else {
                return (intPart + 1) / factor;
            }
        }
    }
private slots:
    void on_saveBtn_clicked();

    void on_openBtn_clicked();

    void on_printBtn_clicked();

    void on_printPartBtn_clicked();

private:
    Ui::WeighUI *ui;
    QString m_testItem;
    QString m_userName;
    QString m_date;
    QDateTime m_startTime;
    QDateTime m_endTime;
    QLabel *m_status;
    double m_constLimit;//恒重判定值
    bool m_isSaved;
};

#endif // WEIGHUI_H
