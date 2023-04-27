#include "mytablemodel.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QFile>
#include <QTextStream>
#include<QItemSelectionModel>
#include<QDataStream>
#include<qDebug>
MyTableModel::MyTableModel(QObject *parent)
    : QAbstractTableModel(parent),
    m_usedRow(0)
{
    // initialize the data
    m_data = QVector<QVector<QVariant>>(99, QVector<QVariant>(8, QVariant()));
}

MyTableModel::~MyTableModel()
{
}



int MyTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.size();
}

int MyTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 8;
}

QVariant MyTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return m_data[index.row()][index.column()];
    }

    return QVariant();
}

QVariant MyTableModel::data(int row, int colunm, int role) const
{
    QModelIndex index = createIndex(row, colunm);
    return data(index,role);
}

bool MyTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

    if (index.isValid() && role == Qt::EditRole) {
        m_data[index.row()][index.column()] = value;
        if(value.isValid()) m_usedRow=qMax(m_usedRow,index.row());
        if(index.column()==2&&value.isValid()) {
            emit checkMediumNum(index,value);//对编号进行检查，确认滤膜已经有称重。
        }
        emit dataChanged(index, index, { role });
        return true;
    }

    return false;
}

bool MyTableModel::setData(int row, int colunm, const QVariant &value, int role)
{
    QModelIndex index = createIndex(row, colunm);
    return setData(index, value,role);
}

QVariant MyTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("样品编号");
        case 1:
            return tr("取样体积");
        case 2:
            return tr("介质编号");
        case 3:
            return tr("备注");
        case 4:
            return tr("第一次");
        case 5:
            return tr("第二次");
        case 6:
            return tr("第三次");
        case 7:
            return tr("第四次");
        default:
            return QVariant();
        }
    }
    if (orientation == Qt::Vertical) {
        return section+1;
    }
    return QVariant();
}

Qt::ItemFlags MyTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

    if (index.column() >= 0 && index.column() < 4) {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

bool MyTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (row < 0 || row > m_data.size())
        return false;

    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        m_data.insert(row + i, QVector<QVariant>(8, QVariant()));
    }
    endInsertRows();

    return true;
}

bool MyTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (row < 0 || row + count > m_data.size())
        return false;

    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        if(row<=m_usedRow) m_usedRow--;
        m_data.remove(row);
    }
    endRemoveRows();

    return true;
}

void MyTableModel::copy()
{
    QClipboard *clipboard = QApplication::clipboard();
    QItemSelectionModel *selectionModel = qobject_cast<QItemSelectionModel *>(QObject::sender());

    if (selectionModel) {
        QModelIndexList indexes = selectionModel->selectedIndexes();
        if (indexes.isEmpty())
            return;

        QMimeData *mimeData = new QMimeData();
        QByteArray encodedData;

        QDataStream stream(&encodedData, QIODevice::WriteOnly);
        for (const QModelIndex &index : indexes) {
            if (index.column() >= 0 && index.column() <= 3) {
                stream << index.row() << index.column() << m_data[index.row()][index.column()].toString() << "\t";
            }
        }

        mimeData->setData("application/x-qabstractitemmodeldatalist", encodedData);
        clipboard->setMimeData(mimeData);
    }
}

void MyTableModel::paste()
{
    QClipboard *clipboard = QApplication::clipboard();
    QItemSelectionModel *selectionModel = qobject_cast<QItemSelectionModel *>(QObject::sender());

    if (selectionModel) {
        QModelIndexList indexes = selectionModel->selectedIndexes();
        if (indexes.isEmpty())
            return;

        const QMimeData *mimeData = clipboard->mimeData();
        if (!mimeData->hasFormat("application/x-qabstractitemmodeldatalist"))
            return;

        QByteArray encodedData = mimeData->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);

        int row, column;
        QString text;
        while (!stream.atEnd()) {
            stream >> row >> column >> text;
            QModelIndex index = createIndex(row, column);
            setData(index, text, Qt::EditRole);
        }
    }
}

void MyTableModel::cut()
{
    copy();
    clear();
}

void MyTableModel::autoFill()
{
    QItemSelectionModel *selectionModel = qobject_cast<QItemSelectionModel *>(QObject::sender());

    if (selectionModel) {
        QModelIndexList indexes = selectionModel->selectedIndexes();
        if (indexes.isEmpty())
            return;

        for (const QModelIndex &index : indexes) {
            if (index.column() >= 0 && index.column() <= 3) {
                setData(index, QVariant(), Qt::EditRole);
            }
        }
    }
}

void MyTableModel::clear()
{

    QItemSelectionModel *selectionModel = qobject_cast<QItemSelectionModel *>(QObject::sender());

    if (selectionModel) {
        QModelIndexList indexes = selectionModel->selectedIndexes();
        if (indexes.isEmpty())
            return;

        for (const QModelIndex &index : indexes) {
            if (index.column() >= 0 && index.column() <= 3) {
                setData(index, QVariant(), Qt::EditRole);
            }
        }
    }
}

bool MyTableModel::canInsertRows(int row, int count) const
{
    return row >= 0 && row <= m_data.size() && count > 0;
}

//void MyTableModel::insertRows(int row, int count)
//{
//    insertRows(row, count, QModelIndex());
//}

void MyTableModel::loadFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QTextStream in(&file);
    int row = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        if (row >= m_data.size()) {
            insertRows(row, 1);
        }

        for (int i = 0; i < fields.size(); ++i) {
            QModelIndex index = createIndex(row, i);
            setData(index, fields[i], Qt::EditRole);
        }

        ++row;
    }

    file.close();
}

void MyTableModel::saveToFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }

    QTextStream out(&file);
    for (int row = 0; row < m_data.size(); ++row) {
        QStringList fields;
        for (int column = 0; column < 7; ++column) {
            fields << m_data[row][column].toString();
        }
        out << fields.join(",") << "\n";
    }

    file.close();
}

