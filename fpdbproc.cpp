#include "fpdbproc.h"

#include "dboper.h"

#include <QStringList>

FpDbProc::FpDbProc(QObject *parent) :
    QObject(parent)
{
    m_pDbOperMem = new DbOper(parent);
    m_pDbOperLocal = new DbOper(parent);
    m_bIsMemPrepared = false;
    m_bIsLocalPrepared = false;
}

FpDbProc::~FpDbProc()
{
    delete m_pDbOperMem;
    delete m_pDbOperLocal;
    m_bIsMemPrepared = false;
    m_bIsLocalPrepared = false;
}

bool FpDbProc::prepareMemDb()
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    if (m_bIsMemPrepared)
    {
        return retRes;
    }

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
            "    day_of_week         INT,              "
            "    punch_hours         DOUBLE,           "
            "    payroll_multi       INT,              "
            "    charge_hours        DOUBLE,           "
            "    punch_type          INT,              "
            "    PRIMARY KEY(ID_number,timeflag)       "
            ")";
    retRes  = retRes && dbOper->dbQureyExec(strSql);

    if (!dbOper->tablesInDb().contains("days_payroll_multi"))
    {
        strSql = "CREATE TABLE payroll_multi ("
                "    multiples  INT NOT NULL,"
                "    descrip    VCHAR DEFAULT '',"
                "    PRIMARY    KEY(multiples)"
                ")";
        retRes  = retRes && dbOper->dbQureyExec(strSql);

        strSql = "INSERT INTO payroll_multi(multiples,descrip) VALUES (1,'工作日');";
        retRes  = retRes && dbOper->dbQureyExec(strSql);
        strSql = "INSERT INTO payroll_multi(multiples,descrip) VALUES (2,'双休日');";
        retRes  = retRes && dbOper->dbQureyExec(strSql);
        strSql = "INSERT INTO payroll_multi(multiples,descrip) VALUES (3,'节假日')";
        retRes  = retRes && dbOper->dbQureyExec(strSql);

        strSql = "CREATE TABLE days_payroll_multi ("
                "    e_date     DATE NOT NULL,"
                "    multiples  INT DEFAULT 0,"
                "    PRIMARY KEY(e_date),"
                "    FOREIGN KEY(multiples) REFERENCES payroll_multi(multiples)"
                ")";
        retRes  = retRes && dbOper->dbQureyExec(strSql);
    }


    if (retRes)
    {
        m_bIsMemPrepared = true;
    }
    return retRes;
}

bool FpDbProc::prepareLocalDb()
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = m_pDbOperLocal;

    if (m_bIsLocalPrepared)
    {
        return retRes;
    }

    retRes = retRes && dbOper->dbOpen("loaclDb.sqlite");
    if (!retRes)
    {
        return retRes;
    }

    if (!dbOper->tablesInDb().contains("days_payroll_multi"))
    {
        strSql = "CREATE TABLE payroll_multi ("
                "    multiples  INT NOT NULL,"
                "    descrip    VCHAR DEFAULT '',"
                "    PRIMARY    KEY(multiples)"
                ")";
        retRes  = retRes && dbOper->dbQureyExec(strSql);

        strSql = "INSERT INTO payroll_multi(multiples,descrip) VALUES (1,'工作日');";
        retRes  = retRes && dbOper->dbQureyExec(strSql);
        strSql = "INSERT INTO payroll_multi(multiples,descrip) VALUES (2,'双休日');";
        retRes  = retRes && dbOper->dbQureyExec(strSql);
        strSql = "INSERT INTO payroll_multi(multiples,descrip) VALUES (3,'节假日')";
        retRes  = retRes && dbOper->dbQureyExec(strSql);

        strSql = "CREATE TABLE days_payroll_multi ("
                "    e_date     DATE NOT NULL,"
                "    multiples  INT DEFAULT 0,"
                "    PRIMARY KEY(e_date),"
                "    FOREIGN KEY(multiples) REFERENCES payroll_multi(multiples)"
                ")";
        retRes  = retRes && dbOper->dbQureyExec(strSql);
    }

    if (retRes)
    {
        m_bIsLocalPrepared = true;
    }
    return retRes;
}

bool FpDbProc::getDutyCollectionFromMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "SELECT DISTINCT poid,job_id,name,SUM(work_hours)"
            " FROM duty_detail WHERE work_hours_type = 0 GROUP BY ID_number,poid ORDER BY poid,job_id";
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

bool FpDbProc::setDutyDetailIntoMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "INSERT INTO duty_detail VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
    return dbOper->dbInsertData(strSql,lstStrLstContent);
}

bool FpDbProc::setWorkDaysIntoMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "INSERT INTO days_payroll_multi VALUES(?,?)";
    return dbOper->dbInsertData(strSql,lstStrLstContent);
}

bool FpDbProc::getWorkDaysFromLocalDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperLocal;

    strSql = "SELECT e_date,multiples"
            " FROM days_payroll_multi"
            " ORDER BY e_date";
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

bool FpDbProc::getWorkDaysFromMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "SELECT e_date,multiples"
            " FROM days_payroll_multi"
            " ORDER BY e_date";
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

bool FpDbProc::setWorkDaysIntoLocalDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperLocal;

    strSql = "INSERT INTO days_payroll_multi VALUES(?,?)";
    return dbOper->dbInsertData(strSql,lstStrLstContent);
}
