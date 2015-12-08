#include "fpdbproc.h"

#include "dboper.h"

#include <QStringList>
#include <QString>
#include <QDateTime>


const QString detailSQLTmp =
        QString("SELECT company,area,product_line,sub_product_line,"
                " PDU_SPDT,job_id,name,on_duty,off_duty,collaboration_type,ID_number,POID"
                " FROM duty_detail %1");

const QString FpDbProc::s_strDetailSQL = detailSQLTmp.arg("");

const QString FpDbProc::s_strDetailBelateOrLeaveEarlySQL =
        detailSQLTmp.arg("WHERE punch_type = 1");

const QString FpDbProc::s_strDetailMissPunchInSQL =
        detailSQLTmp.arg("WHERE punch_type = 2");

//const QString FpDbProc::m_strCollectionSQL =
//        "SELECT * FROM duty_collection";


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
        strSql = "INSERT INTO payroll_multi(multiples,descrip) VALUES (2,'公休日');";
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
    else
    {
        strSql = "DELETE FROM days_payroll_multi";
        retRes  = retRes && dbOper->dbQureyExec(strSql);
    }

    strSql = "DROP TABLE IF EXISTS duty_collection";
    retRes  = retRes && dbOper->dbQureyExec(strSql);

//    strSql = "CREATE TABLE duty_collection (               "
//            "    company             VCHAR,            "
//            "    area                VCHAR,            "
//            "    product_line        VCHAR,            "
//            /*"    year_month          date,             "*/
//            "    POID                VCHAR,            "
//            "    job_id              VCHAR,            "
//            "    name                VCHAR,            "
//            "    punch_hours         DOUBLE,           "
//            "    PRIMARY KEY(job_id,name)       "
//            ")";
//    retRes  = retRes && dbOper->dbQureyExec(strSql);


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

bool FpDbProc::getDutyDistinctPersonalFromMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "SELECT DISTINCT company,area,product_line,poid,ID_number,name, round(SUM(punch_hours),2) as punchIn_h"
            " FROM duty_detail WHERE punch_type IN (0,1) "
            " GROUP BY poid,ID_number ORDER BY poid,name";
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

//bool FpDbProc::setDutyCollectionIntoMemDb(QList<QList<QVariant> > &lstStrLstContent)
//{
//    QString strSql;
//    DbOper *dbOper = m_pDbOperMem;

//    strSql = "INSERT INTO duty_collection VALUES(?,?,?,?,?,?,?)";
//    return dbOper->dbInsertData(strSql,lstStrLstContent);
//}


bool FpDbProc::setDutyDetailIntoMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "INSERT INTO duty_detail VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
    return dbOper->dbInsertData(strSql,lstStrLstContent);
}

bool FpDbProc::setWorkDaysIntoMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    return setWorkDaysIntoDb(lstStrLstContent,m_pDbOperMem);
}

bool FpDbProc::setWorkDaysIntoLocalDb(QList<QList<QVariant> > &lstStrLstContent)
{
    return setWorkDaysIntoDb(lstStrLstContent,m_pDbOperLocal);
}

bool FpDbProc::setWorkDaysIntoDb(QList<QList<QVariant> > &lstStrLstContent, DbOper *argDbOper)
{
    QString strSql;
    DbOper *dbOper = argDbOper;

    strSql = "INSERT INTO days_payroll_multi VALUES(?,?)";
    return dbOper->dbInsertData(strSql,lstStrLstContent);
}

bool FpDbProc::getWorkDaysFromMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    return getWorkDaysFromDb(lstStrLstContent,m_pDbOperMem);
}

bool FpDbProc::getWorkDaysFromLocalDb(QList<QList<QVariant> > &lstStrLstContent)
{
    return getWorkDaysFromDb(lstStrLstContent,m_pDbOperLocal);
}

bool FpDbProc::getWorkDaysFromDb(QList<QList<QVariant> > &lstStrLstContent, DbOper *argDbOper)
{
    QString strSql;
    DbOper *dbOper = argDbOper;

    strSql = "SELECT e_date,multiples"
            " FROM days_payroll_multi"
            " ORDER BY e_date";
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

bool FpDbProc::getWorkDaysByCurMonthFromMemDb(QList<QList<QVariant> > &lstStrLstContent, const QString &strCurMonth)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = QString("SELECT e_date,multiples"
                     " FROM days_payroll_multi"
                     " WHERE strftime('\%Y\%m', e_date) = '%1'"
                     " ORDER BY e_date").arg(strCurMonth);
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

bool FpDbProc::getMinMaxMonthFromMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "SELECT min(timeflag),max(timeflag) FROM duty_detail";
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

bool FpDbProc::getDutyDetailByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                                    const QString &POID,const QString &IDNumber)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = QString("SELECT timeflag,round(punch_hours,2),round(punch_hours*payroll_multi,2) AS charge_h"
                     " FROM duty_detail"
                     " WHERE POID = '%1' AND ID_number = '%2' AND punch_type IN (0,1)"
                     " ORDER BY timeflag").arg(POID).arg(IDNumber);
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

bool FpDbProc::getDutyDetailFromMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = s_strDetailSQL;
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

bool FpDbProc::getDutyDetailBelateOrLeaveEarlyFromMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = s_strDetailBelateOrLeaveEarlySQL;
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

bool FpDbProc::getDutyDetailMissPunchInFromMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = s_strDetailMissPunchInSQL;
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}
