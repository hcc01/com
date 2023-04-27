#include "mytableview.h"
#include "mytablemodel.h"
#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QFileDialog>
#include<QInputDialog>
#include<QRegularExpression>
#include<QDebug>
#include"autofillconfirmdialog.h"
#include<QApplication>
#include<QMimeData>
#include<QClipboard>
MyTableView::MyTableView(QWidget *parent)
    : QTableView(parent)
{
    m_model=new MyTableModel(this);
    setModel(m_model);
//    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    createActions();
    createContextMenu();    
}

MyTableView::~MyTableView()
{
    delete m_contextMenu;
}

void MyTableView::cut()
{
    QModelIndexList indexes = selectionModel()->selectedIndexes();
    if (indexes.empty()) {
        return;
    }
    for (const auto& index : indexes) {

    }
}

void MyTableView::copy()
{

}

void MyTableView::paste()
{

}

void MyTableView::del()
{
    QModelIndexList selection = selectionModel()->selectedIndexes();
    foreach (QModelIndex index, selection) {
        // 处理选定单元格的逻辑
        m_model->setData(index, QVariant(), Qt::EditRole);
    }
}

void MyTableView::clear()
{
    QModelIndexList selection = selectionModel()->selectedIndexes();
    int row0=selection.first().row();
    int column0=selection.first().column();
    for(int r=row0;r<selection.last().row()+1;r++){
        for(int c=column0;c<selection.last().column()+1;c++)
            m_model->setData(r, c,QVariant());
    }
}

QVariant MyTableView::data(int row, int column)
{
    return m_model->data(row,column);
}

void MyTableView::setData(int row, int column, const QVariant &data)
{
    m_model->setData(row,column,data);
}

void MyTableView::clearAll()
{
    selectAll();
    clear();
    clearSelection();
}

QModelIndexList MyTableView::selectedIndexes() const
{
    return QTableView::selectedIndexes();
}

int MyTableView::currentRow()
{
    if(selectedIndexes().isEmpty()) return 0;
    return this->selectedIndexes().first().row();
}

void MyTableView::createActions()
{
    m_insertRowAction = new QAction(tr("插入行"), this);
    connect(m_insertRowAction, &QAction::triggered, this, [this]() {
        auto model = qobject_cast<MyTableModel *>(this->model());
        if (model) {
            int row = currentIndex().row();
            bool ok;
            int count = QInputDialog::getInt(this, tr("Insert row"), tr("Number of rows:"), 1, 1, 20, 1, &ok);
            if (!ok) {
                return;
            }
            model->insertRows(row + 1, count);
        }
    });

    m_deleteRowAction = new QAction(tr("删除行"), this);
    connect(m_deleteRowAction, &QAction::triggered, this, [this]() {
        auto model = qobject_cast<MyTableModel *>(this->model());
        if (model) {
            int row = selectedIndexes().first().row();
            int count=selectedIndexes().last().row()-row+1;
            model->removeRows(row, count);
        }
    });

    m_copyAction = new QAction(tr("复制"), this);
    connect(m_copyAction, &QAction::triggered, this, [this]() {
        QModelIndexList indexes = selectionModel()->selectedIndexes();
        if (indexes.empty()) {
            return;
        }
        int minRow = INT_MAX, maxRow = INT_MIN, minCol = INT_MAX, maxCol = INT_MIN;
        for (const auto& index : indexes) {
            minRow = qMin(minRow, index.row());
            maxRow = qMax(maxRow, index.row());
            minCol = qMin(minCol, index.column());
            maxCol = qMax(maxCol, index.column());
        }
        QMimeData* data = new QMimeData();
        QString text;
        for (int row = minRow; row <= maxRow; row++) {
            for (int col = minCol; col <= maxCol; col++) {
                if (col > minCol) {
                    text += "\t";
                }
                QString item = m_model->data(row, col).toString();
                text += item;
            }
            text += "\n";
        }
        data->setText(text);
        QApplication::clipboard()->setMimeData(data);
    });

    m_pasteAction = new QAction(tr("粘贴"), this);
    connect(m_pasteAction, &QAction::triggered, this, [this]() {
        QModelIndexList indexes = selectionModel()->selectedIndexes();
        if (indexes.empty()) {
            return;
        }
        int minRow = INT_MAX, maxRow = INT_MIN, minCol = INT_MAX, maxCol = INT_MIN;
        for (const auto& index : indexes) {
            minRow = qMin(minRow, index.row());
            maxRow = qMax(maxRow, index.row());
            minCol = qMin(minCol, index.column());
            maxCol = qMax(maxCol, index.column());
        }
        qDebug()<<"minRow"<<minRow;
        qDebug()<<"maxRow"<<maxRow;
         qDebug()<<"minCol"<<minCol;
         qDebug()<<"maxCol"<<maxCol;
        QClipboard* clipboard = QApplication::clipboard();
        const QMimeData* data = clipboard->mimeData();
        if (!data->hasText()) {
            return;
        }
        int row = minRow, col = minCol;
        QStringList lines = data->text().split('\n', Qt::SkipEmptyParts);
        for (const auto& line : lines) {
            QStringList fields = line.split('\t', Qt::SkipEmptyParts);
            for (const auto& field : fields) {
                m_model->setData(row, col, field);
                col++;
                if (col > maxCol) {
                    break;
                }
            }
            row++;
            col=minCol;
            if (row > maxRow) {
                break;
            }
        }
    });

    m_cutAction = new QAction(tr("剪切"), this);
    connect(m_cutAction, &QAction::triggered, this, &MyTableView::cut);

    m_clearAction = new QAction(tr("清除"), this);
    m_clearAction->setShortcut(QKeySequence::Delete);
    connect(m_clearAction, &QAction::triggered, this, [this]() {
        clear();
    });

    m_autoFill = new QAction(tr("自增填充"), this);
    m_autoFill->setShortcut(Qt::CTRL + Qt::Key_F);
    connect(m_autoFill, &QAction::triggered, this, [this]() {
        QModelIndexList selection = selectionModel()->selectedIndexes();

        QString str = m_model->data(selection.first()).toString();
        if(str.isEmpty()){
            return;
        }
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatchIterator it = re.globalMatch(str);
        QStringList numbers;
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QString numberStr = match.captured(1);
            numbers<<numberStr;
        }
        if(numbers.count()>1){
            int i=AutoFillConfirmDialog::getIndex(numbers);
            if(i==-1) i=0;
            int j=0;
            int index,length;


            it = re.globalMatch(str);
            while (it.hasNext()) {
                j++;
                QRegularExpressionMatch match = it.next();
                if(j==i+1){
                 index = match.capturedStart(1);
                 length = match.capturedLength(1);
                 qDebug()<<"j"<<j;
                 qDebug()<<"index"<<index;
                 qDebug()<<"length"<<length;
                 qDebug()<<match.captured(1);
                 break;
                }
            }

            QString number=numbers.at(i);
            int num=number.toInt();
            QString newNumber;
            int row0=selection.first().row()+1;
            int column=selection.first().column();
            for(int r=row0;r<selection.last().row()+1;r++){
                num++;
                newNumber=QString::number(num).rightJustified(number.length(), '0');
                QString newStr=str;
                newStr.replace(index, length, newNumber);
                m_model->setData(r, column,newStr);
            }
        }
        else{
            QString number=numbers.at(0);
            int num=number.toInt();
            QString newNumber;
            int row0=selection.first().row()+1;
            int column=selection.first().column();
            for(int r=row0;r<selection.last().row()+1;r++){
                num++;
                newNumber=QString::number(num).rightJustified(number.length(), '0');
                QString newStr=str;
                newStr.replace(number, newNumber);
                m_model->setData(r, column,newStr);
            }
        }
//        auto model = qobject_cast<MyTableModel *>(this->model());
//        if (model) {
//            model->clear();
//        }
    });
    m_copyFill=new QAction(tr("复制填充"), this);
    m_copyFill->setShortcut(Qt::CTRL + Qt::Key_D);
     connect(m_copyFill, &QAction::triggered, this, [this]() {
         QModelIndexList selection = selectionModel()->selectedIndexes();
        int rowStart=selection.first().row();
        int columnStart=selection.first().column();
        int rowEnd=selection.last().row();
        int columnEnd=selection.last().column();
        for(int c=columnStart;c<=columnEnd;c++){
            QVariant v=data(rowStart,c);
            if(v.isValid())
            for(int r=rowStart+1;r<=rowEnd;r++){
                setData(r,c,v);
            }
        }
     });

    m_saveAction = new QAction(tr("保存"), this);
    connect(m_saveAction, &QAction::triggered, this, [this]() {
        QString fileName= QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("CSV Files (*.csv)"));
        if (!fileName.isEmpty()) {
            auto model = qobject_cast<MyTableModel *>(this->model());
            if (model) {
                model->saveToFile(fileName);
            }
        }
    });

    m_loadAction = new QAction(tr("打开"), this);
    connect(m_loadAction, &QAction::triggered, this, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("CSV Files (*.csv)"));
        if (!fileName.isEmpty()) {
            auto model = qobject_cast<MyTableModel *>(this->model());
            if (model) {
                model->loadFromFile(fileName);
            }
        }
    });
}

void MyTableView::createContextMenu()
{
    m_contextMenu = new QMenu(this);
    addAction(m_clearAction);
    addAction(m_pasteAction);
    addAction(m_autoFill);
    m_contextMenu->addAction(m_insertRowAction);
    m_contextMenu->addAction(m_deleteRowAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_copyAction);
    m_contextMenu->addAction(m_pasteAction);
    m_contextMenu->addAction(m_cutAction);
    m_contextMenu->addAction(m_clearAction);
    m_contextMenu->addSeparator();
//    m_contextMenu->addAction(m_saveAction);
//    m_contextMenu->addAction(m_loadAction);
    m_contextMenu->addAction(m_autoFill);
    m_contextMenu->addAction(m_copyFill);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &MyTableView::customContextMenuRequested, this, [this](const QPoint &pos) {
        m_contextMenu->exec(mapToGlobal(pos));
    });
}

void MyTableView::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
}
