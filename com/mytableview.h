
#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H
#include <QTableView>
#include <QMenu>
class MyTableModel;
class MyTableView: public QTableView
{
    Q_OBJECT

public:
    explicit MyTableView(QWidget *parent = nullptr);
    ~MyTableView() override;

    QVariant data(int row,int column);
    void setData(int row,int column,const QVariant& data);
    void clearAll();
    QModelIndexList selectedIndexes()const;
    int currentRow();
public slots:
    void cut();
    void copy();
    void paste();
    void del();
    void clear();
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    QMenu *m_contextMenu;

    QAction *m_insertRowAction;
    QAction *m_deleteRowAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_cutAction;
    QAction *m_clearAction;
    QAction *m_saveAction;
    QAction *m_loadAction;
    QAction *m_autoFill;
    QAction *m_copyFill;
    MyTableModel* m_model;
    void createActions();
    void createContextMenu();
};
#endif // MYTABLEVIEW_H
