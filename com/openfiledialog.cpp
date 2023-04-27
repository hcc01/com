#include "openfiledialog.h"
#include "ui_openfiledialog.h"
#include"dbmanager.h"
#include<QSqlTableModel>
#include<QSqlQueryModel>
OpenFileDialog::OpenFileDialog(const QString& user,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenFileDialog),
    m_user(user)
{
    ui->setupUi(this);

    QSqlQuery query;
    if(!query.exec(QString("select saveMark as 文件名 ,weightDate as 分析日期 from records where operator='%1' group by saveMark,weightDate ORDER BY weightDate DESC").arg(m_user))){
        qDebug()<<"查看记录表格错误："<<query.lastError().text();
    }


    QSqlQueryModel* model=new QSqlQueryModel(this);
    model->setQuery(query);
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
}

OpenFileDialog::~OpenFileDialog()
{
    delete ui;
}

QList<QVariant> OpenFileDialog::getSelectedData(const QString& user)
{
    OpenFileDialog dialog(user);
    int result = dialog.exec();
    if (result == QDialog::Accepted) {
        return dialog.m_selectedData;
    } else {
        return {};
    }
}

void OpenFileDialog::on_buttonBox_accepted()
{
    // 获取当前选择单元格的索引
    QModelIndex currentIndex = ui->tableView->currentIndex();

    // 获取单元格所在的行号
    int row = currentIndex.row();

    // 获取数据模型
    QAbstractItemModel *model = ui->tableView->model();

    // 遍历整行的单元格
    QVariantList rowData;
    for (int i = 0; i < model->columnCount(); i++) {
        // 获取单元格的索引
        QModelIndex index = model->index(row, i);

        // 获取单元格的值
        QVariant value = model->data(index, Qt::DisplayRole);

        // 将值添加到行数据列表中
        rowData.append(value);
    }

    // 输出行数据
    m_selectedData= rowData;
}

