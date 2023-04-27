
#include "dbmanager.h"

QSqlQuery DBManager::query(const QString &q,bool &ok)
{
    QSqlQuery query;
    ok=query.exec(q);
    if(!ok){
        m_lastError=query.lastError().text();
    }
    return query;
}

DBManager::DBManager()
{
    // 创建数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");
    QSqlQuery query(db);
    if (!db.open()) {
         m_lastError=query.lastError().text();
        qDebug()<<m_lastError;
        return ;
    }

    // 创建用户表格
    if (!query.exec("CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password TEXT)")) {
        m_lastError=query.lastError().text();qDebug()<<m_lastError;
        return ;
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS logininfo (username TEXT , logindate DATETIME)")) {
        m_lastError=query.lastError().text();qDebug()<<m_lastError;
        return ;
    }
    //分析记录表
    if(!query.exec("CREATE TABLE IF NOT EXISTS records ("
                    "id integer "
                    "primary key,"
                    "sampleNum varchar(32), "//样品编号，介质称量记录为空
                    "sampleAmount varchar(32),"//样品体积，介质称量记录为空
                    "mediumNum varchar(32),"//滤膜编号
                    "mediumState boolean,"//滤膜使用情况，用于介质准备记录表
                    "marks TEXT,"//备注
                    "result1 DOUBLE,"//第一次
                    "result2 DOUBLE,"
                    "result3 DOUBLE,"
                    "result4 DOUBLE,"
                    "sampleType varchar(32),"
                    "mediumType integer,"//滤膜类型，对应控件中的index
                    "temperature DOUBLE ,"
                    "humidity DOUBLE ,"//湿度
                    "operator varchar(32),"
                    "saveMark TEXT,"//保存备注（相当于文件名，用于索引时打开记录）
                    "weightDate varchar(32),"
                    "startTime DATETIME,"//记录开始称量时间和结束称量时间
                    "endTime DATETIME);"
                    "")) {
        m_lastError="创建记录表时出错："+query.lastError().text();qDebug()<<m_lastError;
        return ;
    }
    // 添加默认管理员用户
    if (!query.exec("SELECT COUNT(*) FROM users")) {
         m_lastError=query.lastError().text();qDebug()<<m_lastError;
        return;
    }
    if (query.next() && query.value(0).toInt() == 0) {
        if (!query.exec("INSERT INTO users (username, password) VALUES ('admin', '123')")) {
             m_lastError=query.lastError().text();qDebug()<<m_lastError;
            return;
        }
    }
}

