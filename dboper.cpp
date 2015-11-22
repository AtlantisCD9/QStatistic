#include "dboper.h"
#include <QSqlDatabase>
#include <QDebug>

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

DbOper *DbOper::m_pInstance = NULL;

DbOper *DbOper::getInstance(QObject *parent)
{
    if(m_pInstance == NULL)//判断是否第一次调用
    {
        m_pInstance = new DbOper(parent);
    }
    return m_pInstance;
}

void DbOper::releaseInstance()
{
    if(NULL != m_pInstance)//判断是否已经释放
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

DbOper::DbOper(QObject *parent) :
    QObject(parent)
{
    m_pSqlDb = new QSqlDatabase;
}
DbOper::~DbOper()
{
    delete m_pSqlDb;
}

bool DbOper::dbOpen()
{
    if (m_pSqlDb->isOpen())
    {
        return true;
    }

    *m_pSqlDb = QSqlDatabase::addDatabase("QSQLITE");
    //m_pSqlDb->setDatabaseName(":memory:");
    m_pSqlDb->setDatabaseName("customdb");
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

bool DbOper::dbInsertData(QString &sqlStr, QList<QList<QVariant> > &lstRowsAllData)
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
        return false;
    }

    return true;
}
