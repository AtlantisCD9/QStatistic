#ifndef FPDBPROC_H
#define FPDBPROC_H

#include <QObject>
#include <QDate>

class DbOper;

class FpDbProc : public QObject
{
    Q_OBJECT
public:
    explicit FpDbProc(QObject *parent = 0);
    ~FpDbProc();

    //初始化内存数据库信息
    bool prepareMemDb();
    //初始化文本数据库信息
    bool prepareLocalDb();


    //初始化工时明细表
    bool initDutyDetailMemDb();
    //初始化异常工时处理表
    bool initProcAbnormalDetailMemDb();
    //初始化月结汇总表
    bool initDutyPersonalSumMemDb();
    //初始化PO切换表
    bool initPoSwitchMemDb();

    //将工时明细导入内存数据库
    bool setDutyDetailIntoMemDb(QList<QList<QVariant> > &lstStrLstContent,QString &dbErrorInfo);
    //将异常工时处理导入内存数据库
    bool setProcAbnormalDetailIntoMemDb(QList<QList<QVariant> > &lstStrLstContent,QString &dbErrorInfo);
    //将Po切换信息内存数据库
    bool setPoSwitchIntoMemDb(QList<QList<QVariant> > &lstStrLstContent,QString &dbErrorInfo);

    //刷新加班工时，超过8小时的，按照8小时计算，同时刷新折现工时
    bool updateDutyOverHours();
    //结合明细表和异常工时处理表，获取异常工时处理人员工班信息
    bool getBaseInfoInProcAbnormalDetailFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    //由身份证号码，获取异常工时处理表中不再明细表的工班标识
    bool getTimeFlagNotInDetailInProcAbnormalDetailFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                                             const QString &POID, const QString &IDNumber);
    //根据异常工时处理内容，刷新工时明细表格
    bool updateDutyDetailByProcAbnormalDetail();
    //根据工时明细，刷新月结汇总表基础信息
    bool updateDutyPersonalSumByDutyDetailMemDb();
    //刷新月结汇总表的起止时间
    bool updateDutyPersonalSumSetDateInterMemDb(QDate startDate,QDate endDate);
    //根据PO切换信息刷新月结汇总表的起止时间
    bool updateDutyPersonalSumByPoSwitchMemDb();

    //从内存数据库取出月结汇总表
    bool getDutyPersonalSumFromMemDb(QList<QList<QVariant> > &lstStrLstContent);

    //设置工作日节假日配置表到内存数据库
    bool setWorkDaysIntoMemDb(QList<QList<QVariant> > &lstStrLstContent);
    //设置工作日节假日配置表到文件数据库
    bool setWorkDaysIntoLocalDb(QList<QList<QVariant> > &lstStrLstContent);
    //从内存数据库中获取工作日节假日配置表内容
    bool getWorkDaysFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    //从文件数据库中获取工作日节假日配置表内容
    bool getWorkDaysFromLocalDb(QList<QList<QVariant> > &lstStrLstContent);

    //从内存数据库中获取当月工作日节假日配置表内容
    bool getWorkDaysByCurMonthFromMemDb(QList<QList<QVariant> > &lstStrLstContent,const QString &strCurMonth);
    //查询内存数据库中最小最大日期，用于检查导入到表格是否跨月度
    bool getMinMaxMonthFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    //由POID和身份证号码，查询打卡工时和异常工时日期明细
    bool getDutyDetailByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                          const QString &POID, const QString &IDNumber);
    //由POID和身份证号码，查询工作日异常工时汇总
    bool getDutyAbnormalHoursSumByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                          const QString &POID, const QString &IDNumber);
    //由POID和身份证号码，查询工作日正常打卡工时汇总
    bool getDutyPunchInHoursSumByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                          const QString &POID, const QString &IDNumber);
    //由POID和身份证号码，查询公休日和节假日打卡工时汇总
    bool getDutyOverHoursSumByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                          const QString &POID, const QString &IDNumber);
    //从内存数据库根据PO和身份证号码，获取汇总起止时间
    bool getDutyPersDateByPoIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                                const QString &POID, const QString &IDNumber);

    //查询明细工时信息
    bool getDutyDetailFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    //查询迟到早退工时信息
    bool getDutyDetailBelateOrLeaveEarlyFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    //查询上下岗记录丢失工时信息
    bool getDutyDetailMissPunchInFromMemDb(QList<QList<QVariant> > &lstStrLstContent);


public:
    const static QString s_strDetailSQL;
    const static QString s_strDetailBelateOrLeaveEarlySQL;
    const static QString s_strDetailMissPunchInSQL;
//    const static QString m_strCollectionSQL;

private:
    bool getWorkDaysFromDb(QList<QList<QVariant> > &lstStrLstContent,DbOper *argDbOper);
    bool setWorkDaysIntoDb(QList<QList<QVariant> > &lstStrLstContent,DbOper *argDbOper,QString &dbErrorInfo);

    //初始化工时明细表
    bool initDutyDetailDb(DbOper *argDbOper);
    //初始化异常工时处理表
    bool initProcAbnormalDetailDb(DbOper *argDbOper);
    //初始化月结汇总表
    bool initDutyPersonalSumDb(DbOper *argDbOper);
    //初始化PO切换表
    bool initPoSwitchDb(DbOper *argDbOper);
    //初始化工作日公休日节假日配置表
    bool initDaysPayrollMultiDb(DbOper *argDbOper);
    //初始化员工信息表
    bool initEmployeeInfoDb(DbOper *argDbOper);


private:
    DbOper *m_pDbOperMem;
    DbOper *m_pDbOperLocal;

    bool m_bIsMemPrepared;//是否初始化标识
    bool m_bIsLocalPrepared;//是否初始化标识你

};

#endif // FPDBPROC_H
