#include "dboper.h"
#include <QSqlDatabase>
#include <QDebug>

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QStringList>

DbOper::DbOper(QObject *parent) :
    QObject(parent)
{
    m_pSqlDb = new QSqlDatabase;
}
DbOper::~DbOper()
{
    delete m_pSqlDb;
}

bool DbOper::dbOpen(const QString databaseName)
{
    if (m_pSqlDb->isOpen())
    {
        return true;
    }

    *m_pSqlDb = QSqlDatabase::addDatabase("QSQLITE",databaseName);
    m_pSqlDb->setDatabaseName(databaseName);
    return m_pSqlDb->open();
}

bool DbOper::dbQureyExec(QString &sqlStr)
{
    QSqlQuery sqlQuery(*m_pSqlDb);

    if (!sqlQuery.exec(sqlStr))
    {
        qDebug() << sqlQuery.lastError().text();
        return false;
    }

    return true;
}

bool DbOper::dbQureyData(QString &sqlStr, QList<QList<QVariant> > &lstRowsAllData)
{
    QSqlQuery sqlQuery(*m_pSqlDb);

    if (!sqlQuery.exec(sqlStr))
    {
        qDebug() << sqlQuery.lastError().text();
        return false;
    }

    QSqlRecord rec = sqlQuery.record();
    //qDebug() << "Number of columns: " << rec.count();

    while (sqlQuery.next())
    {
        lstRowsAllData << QList<QVariant>();
        for (int i=0;i<rec.count();++i)
        {
            lstRowsAllData.last() << sqlQuery.value(i);
        }
    }

    return true;
}

bool DbOper::dbInsertData(QString &sqlStr, QList<QList<QVariant> > &lstRowsAllData, QString &dbErrorInfo)
{
    QSqlQuery sqlQuery(*m_pSqlDb);
    sqlQuery.prepare(sqlStr);

    QList<QList<QVariant> > lstColumnsAllData;
    //initial lstColumnsAllData
    if(lstRowsAllData.size() != 0)
    {
        for(int i=0;i<lstRowsAllData.first().size();++i)
        {
            lstColumnsAllData << QList<QVariant>();
        }
    }

    foreach(QList<QVariant> rowsData,lstRowsAllData)
    {
        for(int i=0;i<rowsData.size();++i)
        {
            lstColumnsAllData[i] << rowsData[i];
        }
    }

    foreach(QList<QVariant> columnsData,lstColumnsAllData)
    {
        sqlQuery.addBindValue(columnsData);
    }

    if (!sqlQuery.execBatch(QSqlQuery::ValuesAsRows))
    {
        qDebug() << sqlQuery.lastError();
        dbErrorInfo = sqlQuery.lastError().text();
        return false;
    }

    return true;
}

QStringList DbOper::tablesInDb()
{
    //SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;
    return m_pSqlDb->tables();
}
