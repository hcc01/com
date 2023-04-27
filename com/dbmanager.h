
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include<QDebug>
#include<QObject>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>
class DBManager
{

public:
    static DBManager& DM(){
        static DBManager dm;
        return dm;
    }
    QString lastError(){
        return m_lastError;
    }
    QSqlDatabase& db(){return m_db;}
    QSqlQuery query(const QString& q, bool &ok);
private:
    DBManager();
    QString m_lastError;
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
