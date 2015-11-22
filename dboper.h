#ifndef DBOPER_H
#define DBOPER_H

#include <QObject>

class QSqlDatabase;

class DbOper : public QObject
{
    Q_OBJECT

public:
    ~DbOper();
    static DbOper *getInstance(QObject *parent = 0);
    static void releaseInstance();


    bool dbOpen();
    bool dbQureyExec(QString &sqlStr);

    bool dbQureyData(QString &sqlStr,QList<QList<QVariant> > &lstRowsAllData);
    bool dbInsertData(QString &sqlStr, QList<QList<QVariant> > &lstRowsAllData);

    //QSqlDatabase *getSqlDb(){return m_pSqlDb;}

private:
    explicit DbOper(QObject *parent = 0);
    static DbOper *m_pInstance;
    QSqlDatabase *m_pSqlDb;



    
signals:
    
public slots:
    
};

#endif // DBOPER_H
