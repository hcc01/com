
#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

#include <QAbstractTableModel>

class MyTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MyTableModel(QObject *parent = nullptr);
    ~MyTableModel() override;
    int getUsedRow(){return m_usedRow;}
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant data(int row,int colunm, int role = Qt::DisplayRole) const ;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool setData(int row,int colunm, const QVariant &value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void copy();
    void paste();
    void cut();
    void autoFill();
    void clear();


    bool canInsertRows(int row, int count) const;
    //    void insertRows(int row, int count);

    void loadFromFile(const QString &fileName);
    void saveToFile(const QString &fileName);
signals:
    void checkMediumNum(const QModelIndex &index,const QVariant&);
private:
    QVector<QVector<QVariant>> m_data;
    int m_usedRow;
};


#endif // MYTABLEMODEL_H
