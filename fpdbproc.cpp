#include "fpdbproc.h"

#include "dboper.h"

#include <QStringList>


FpDbProc *FpDbProc::m_pInstance = NULL;

FpDbProc *FpDbProc::getInstance(QObject *parent)
{
    if(m_pInstance == NULL)//判断是否第一次调用
    {
        m_pInstance = new FpDbProc(parent);
    }
    return m_pInstance;
}

void FpDbProc::releaseInstance()
{
    if(NULL != m_pInstance)//判断是否已经释放
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

FpDbProc::FpDbProc(QObject *parent) :
    QObject(parent)
{
    m_pDbOperMem = new DbOper(parent);
    m_pDbOperLocal = new DbOper(parent);
}

FpDbProc::~FpDbProc()
{
    if (NULL != m_pDbOperMem)
    {
        delete m_pDbOperMem;
        m_pDbOperMem = NULL;
    }

    if (NULL != m_pDbOperLocal)
    {
        delete m_pDbOperLocal;
        m_pDbOperLocal = NULL;
    }
}

bool FpDbProc::prepareMemDb()
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    retRes = retRes && dbOper->dbOpen();
    if (!retRes)
    {
        return retRes;
    }

    strSql = "DROP TABLE IF EXISTS duty_detail";
    retRes  = retRes && dbOper->dbQureyExec(strSql);

    strSql = "CREATE TABLE duty_detail (               "
            "    company             VCHAR,            "
            "    area                VCHAR,            "
            "    product_line        VCHAR,            "
            "    sub_product_line    VCHAR,            "
            "    PDU_SPDT            VCHAR,            "
            "    job_id              VCHAR,            "
            "    name                VCHAR,            "
            "    on_duty             DATETIME,         "
            "    off_duty            DATETIME,         "
            "    collaboration_type  VCHAR,            "
            "    ID_number           VCHAR Not Null,   "
            "    POID                VCHAR,            "
            "    timeflag            DATETIME,         "
            "    work_hours          DOUBLE,           "
            "    day_of_week         INT,              "
            "    work_hours_type     INT,              "
            "    PRIMARY KEY(ID_number,timeflag)       "
            ")";
    retRes  = retRes && dbOper->dbQureyExec(strSql);

    return retRes;
}

bool FpDbProc::prepareLocalDb()
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = m_pDbOperLocal;

    retRes = retRes && dbOper->dbOpen("loaclDb.sqlite");
    if (!retRes)
    {
        return retRes;
    }

    if (!m_pDbOperLocal->tablesInDb().contains("work_days"))
    {
        strSql = "CREATE TABLE days_type ("
                "    type    INT NOT NULL,"
                "    descrip VCHAR DEFAULT '',"
                "    PRIMARY KEY(type)"
                ")";
        retRes  = retRes && dbOper->dbQureyExec(strSql);

        strSql = "CREATE TABLE work_days ("
                "    e_date  DATE NOT NULL,"
                "    type    INT DEFAULT 0,"
                "    PRIMARY KEY(e_date),"
                "    FOREIGN KEY(type) REFERENCES days_type(type)"
                ")";
        retRes  = retRes && dbOper->dbQureyExec(strSql);
    }

    return retRes;
}

bool FpDbProc::getDataFromMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "SELECT DISTINCT poid,job_id,name,SUM(work_hours) "
            "FROM duty_detail WHERE work_hours_type = 0 GROUP BY ID_number,poid ORDER BY poid,job_id";
    retRes = retRes && dbOper->dbQureyData(strSql,lstStrLstContent);

//    foreach(QList<QVariant> lstRes,lstStrLstContent)
//    {
//        qDebug() << lstRes;
//    }



    return retRes;

}

bool FpDbProc::setDataIntoMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "INSERT INTO duty_detail VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
    retRes = retRes && dbOper->dbInsertData(strSql,lstStrLstContent);

    return retRes;



//    QList<QList<QVariant> > lstResult;
//    strSql = QString("select * from duty_detail");
//    dbOper->dbQurey(strSql,lstResult);
//    qDebug() << lstResult;



//    DbOper::releaseInstance();


}
