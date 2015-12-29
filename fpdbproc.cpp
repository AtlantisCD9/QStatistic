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
        detailSQLTmp.arg("WHERE punch_type = 1 AND payroll_multi = 1");

const QString FpDbProc::s_strDetailMissPunchInSQL =
        detailSQLTmp.arg("WHERE punch_type = 2 AND payroll_multi = 1");

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

bool FpDbProc::initDutyDetailDb(DbOper *argDbOper)
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = argDbOper;

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
            "    punch_type          INT,              "
            "    abnormal_hours      DOUBLE,           "
            "    punch_hours         DOUBLE,           "
            "    payroll_multi       INT,              "
            "    charge_hours        DOUBLE,           "
            "    PRIMARY KEY(ID_number,timeflag)       "
            ")";
    retRes  = retRes && dbOper->dbQureyExec(strSql);

    return retRes;
}

bool FpDbProc::initProcAbnormalDetailDb(DbOper *argDbOper)
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = argDbOper;

    strSql = "DROP TABLE IF EXISTS proc_abnormal_detail";
    retRes  = retRes && dbOper->dbQureyExec(strSql);

    strSql = "CREATE TABLE proc_abnormal_detail (      "
            "    POID                VCHAR,            "
            "    name                VCHAR,            "
            "    ID_number           VCHAR Not Null,   "
            "    on_duty             DATETIME,         "
            "    off_duty            DATETIME,         "
            "    remark              VCHAR,            "
            "    timeflag            DATETIME,         "
            "    punch_type          INT,              "
            "    abnormal_hours      DOUBLE,           "
            "    punch_hours         DOUBLE,           "
            "    PRIMARY KEY(ID_number,timeflag)       "
            ")";
    retRes  = retRes && dbOper->dbQureyExec(strSql);

    return retRes;
}

bool FpDbProc::initDaysPayrollMultiDb(DbOper *argDbOper)
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = argDbOper;

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
//    else
//    {
//        strSql = "DELETE FROM days_payroll_multi";
//        retRes  = retRes && dbOper->dbQureyExec(strSql);
//    }

    return retRes;
}

bool FpDbProc::initEmployeeInfoDb(DbOper *argDbOper)
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = argDbOper;

    if (!dbOper->tablesInDb().contains("employee_info"))
    {
        //do not in use
        strSql = "CREATE TABLE employee_info ("
                "   identity_card   VCHAR NOT NULL,"
                "   name            VCHAR NOT NULL,"
                "   rt_jobid        VCHAR NOT NULL,"
                "   hw_jobid        VCHAR DEFAULT '',"
                "   PRIMARY         KEY(identity_card)"
                ")";
        retRes  = retRes && dbOper->dbQureyExec(strSql);
    }

    return retRes;
}

bool FpDbProc::prepareMemDb()
{
    bool retRes = true;
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

    retRes = retRes && initDutyDetailDb(dbOper);

    retRes = retRes && initProcAbnormalDetailDb(dbOper);

    retRes = retRes && initDaysPayrollMultiDb(dbOper);

    retRes = retRes && initEmployeeInfoDb(dbOper);

    if (retRes)
    {
        m_bIsMemPrepared = true;
    }
    return retRes;
}

bool FpDbProc::prepareLocalDb()
{
    bool retRes = true;
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

    retRes = retRes && initDaysPayrollMultiDb(dbOper);

    retRes = retRes && initEmployeeInfoDb(dbOper);

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

    strSql = "SELECT DISTINCT company,area,product_line,poid,ID_number,name"
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

bool FpDbProc::initDutyDetailMemDb()
{
    return initDutyDetailDb(m_pDbOperMem);
}

bool FpDbProc::initProcAbnormalDetailMemDb()
{
    return initProcAbnormalDetailDb(m_pDbOperMem);
}

bool FpDbProc::setDutyDetailIntoMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "INSERT INTO duty_detail VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
    return dbOper->dbInsertData(strSql,lstStrLstContent);
}

bool FpDbProc::setProcAbnormalDetailIntoMemDb(QList<QList<QVariant> > &lstStrLstContent)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "INSERT INTO proc_abnormal_detail VALUES(?,?,?,?,?,?,?,?,?,?)";
    return dbOper->dbInsertData(strSql,lstStrLstContent);
}

bool FpDbProc::updateDutyOverHours()
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "UPDATE duty_detail SET punch_hours = 8.0,charge_hours = 8.0*payroll_multi WHERE punch_hours >8.0 AND payroll_multi IN (2,3)";
    return dbOper->dbQureyExec(strSql);
}

bool FpDbProc::updateDutyDetailByProcAbnormalDetail()
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = "REPLACE INTO duty_detail "
            "SELECT a.company,a.area,a.product_line,a.sub_product_line,a.PDU_SPDT,a.job_id, "
            "            a.name,b.on_duty,b.off_duty,a.collaboration_type,a.ID_number,a.POID, "
            "            a.timeflag,a.day_of_week,b.punch_type,b.abnormal_hours,b.punch_hours,a.payroll_multi,a.charge_hours "
            "            FROM duty_detail AS a,proc_abnormal_detail AS b "
            "WHERE strftime('%Y-%m-%d', a.timeflag)  = strftime('%Y-%m-%d', b.timeflag) AND a.ID_number = b.ID_number";
    return dbOper->dbQureyExec(strSql);
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

    strSql = QString("SELECT timeflag,round(punch_hours,2),round(abnormal_hours,2)"
                     " FROM duty_detail"
                     " WHERE POID = '%1' AND ID_number = '%2' AND punch_type IN (0,1)"
                     " ORDER BY timeflag").arg(POID).arg(IDNumber);
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

bool FpDbProc::getDutyAbnormalHoursSumByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                                    const QString &POID,const QString &IDNumber)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = QString("SELECT round(sum(abnormal_hours),2)"
                     " FROM duty_detail"
                     " WHERE POID = '%1' AND ID_number = '%2' AND payroll_multi = 1").arg(POID).arg(IDNumber);
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

bool FpDbProc::getDutyPunchInHoursSumByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                                    const QString &POID,const QString &IDNumber)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = QString("SELECT round(sum(punch_hours),2)"
                     " FROM duty_detail"
                     " WHERE POID = '%1' AND ID_number = '%2' AND payroll_multi = 1").arg(POID).arg(IDNumber);
    return dbOper->dbQureyData(strSql,lstStrLstContent);
}

bool FpDbProc::getDutyOverHoursSumByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                                    const QString &POID,const QString &IDNumber)
{
    QString strSql;
    DbOper *dbOper = m_pDbOperMem;

    strSql = QString("SELECT round(sum(punch_hours),2)"
                     " FROM duty_detail"
                     " WHERE POID = '%1' AND ID_number = '%2' AND payroll_multi IN (2,3)").arg(POID).arg(IDNumber);
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
