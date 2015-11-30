#ifndef DBOPER_H
#define DBOPER_H

#include <QObject>

#include "globaldef.h"

class QSqlDatabase;

class DbOper : public QObject
{
    Q_OBJECT

public:
    explicit DbOper(QObject *parent = 0);
    ~DbOper();

    bool dbOpen(const QString databaseName=DB_NAME);
    bool dbQureyExec(QString &sqlStr);

    bool dbQureyData(QString &sqlStr,QList<QList<QVariant> > &lstRowsAllData);
    bool dbInsertData(QString &sqlStr, QList<QList<QVariant> > &lstRowsAllData);

    QStringList tablesInDb();

    //QSqlDatabase *getSqlDb(){return m_pSqlDb;}

private:
    QSqlDatabase *m_pSqlDb;

};

#endif // DBOPER_H
