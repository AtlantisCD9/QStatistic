#ifndef FPDATAPROC_H
#define FPDATAPROC_H

#include <QObject>
#include <QDateTime>

#include "globaldef.h"

class FpDbProc;
class FpExcelProc;

class FpDataProc : public QObject
{
    Q_OBJECT
public:
    explicit FpDataProc(QObject *parent = 0);
    ~FpDataProc();

    void initial(ENUM_IMPORT_XLS_TYPE importType);

    //Data proc
    void procDataForDatail();
    void procDataAbnormal();
    void procDataForCollection();

    //Format proc
    void procDataFormat(ENUM_IMPORT_XLS_TYPE import_type);

    //Excel proc
    bool getDataFromExcel(QString &inPutFile, int &titleEnd, int &sheetID, int &columnNum,
                          ENUM_IMPORT_XLS_TYPE import_type);
    void setDataIntoExcel(QString &outPutFile,int &sheetID);
    void mergeExcel(QStringList &lstMergeFile, QString &outPutFile, int &titleEnd, int &sheetID, int &columnNum);

    //Loacl Db

    //Mem Db
    //在刷新异常工时处理表到明细工时之前，补齐明细工时中不存在的工时
    void addInfoIntoDutyDetailByProcAbnormalDetail();
    //根据异常工时处理内容，刷新工时明细表格
    void updateDutyDetailByProcAbnormalDetail();
    //根据工时明细，刷新月结汇总表基础信息
    void updateDutyPersonalSumByDutyDetailMemDb();
    //刷新月结汇总表的起止时间
    void updateDutyPersonalSumSetDateInterMemDb(QDate startDate,QDate endDate);
    //根据PO切换信息刷新月结汇总表的起止时间
    void updateDutyPersonalSumByPoSwitchMemDb();

    void setProcAbnormalDetail();
    void setPoSwtich();

    void setDutyDetail();
    const QList<QVariant> &getDutyDetailTitle(){return m_lstTitleDetail;}
    const QString &getDutyDetailSQL();
    const QString &getDutyDetailBelateOrLeaveEarlySQL();
    const QString &getDutyDetailMissPunchInSQL();

    void getBelateOrLeaveEarlyDetail();
    void getMissPunchInDetail();
    void getDutyPersonalSum();

    //void setDutyCollection();
    const QList<QVariant> &getDutyColletionTitle(){return m_lstTitleCollection;}
    const QList<QList<QVariant> > &getDutyColletionLstRowLstColumn(){return m_lstRowLstColumnCollection;}
//    const QString &getDutyColletionSQL();

    void syncWorkDays();

    int getDetailSize(){return m_lstRowLstColumnDetail.size();}
    int getAbnormalSize(){return m_lstRowLstColumnAbnormal.size();}
    int getPoSwitchSize(){return m_lstRowLstColumnPOSwitch.size();}

    bool getAndCheckCurMonth(QDate &startDate,QDate &endDate);//example:yyyyMM

private:
    //Loacl Db
    void getWorkDays();
    void createWorkDays();

    double getAbnormalHourUnit(int secsTemp);
    QDateTime getTimeFlag(QDateTime dtStart);
    int getPunchType(QDateTime dtTimeFlag,QDateTime dtStart,QDateTime dtEnd);
    double getAbnormalHourFNL(QDateTime dtTimeFlag,QDateTime dtStart,QDateTime dtEnd,int punchType,int payrollMulti);
    double getPunchHour(QDateTime dtTimeFlag,QDateTime dtStart,QDateTime dtEnd,int payrollMulti);


    int getAbnormalHours(QDateTime dtTimeFlag,QDateTime dtStart,QDateTime dtEnd);
    int getPunchInHours(QDateTime dtTimeFlag,QDateTime dtStart,QDateTime dtEnd);
    int getOverTimeHours(QDateTime dtTimeFlag,QDateTime dtStart,QDateTime dtEnd);
    
private:
    FpDbProc *m_pFpDbProc;
    FpExcelProc *m_pFpExcelProc;
    QString m_strDateMonth;//format:201510

    QList<QVariant> m_lstTitleDetail;//明细抬头
    QList<QVariant> m_lstTitleCollection;//汇总抬头

    QList<QVariant> m_lstTitleAbnormal;//异常处理表抬头
    QList<QList<QVariant> > m_lstRowLstColumnAbnormal;//异常处理表明细

    QList<QVariant> m_lstTitlePOSwitch;//PO切换表抬头
    QList<QList<QVariant> > m_lstRowLstColumnPOSwitch;//PO切换表明细


    QList<QList<QVariant> > m_lstRowLstColumnDetail;//will be add some proc data

    QList<QList<QVariant> > m_lstRowLstColumnBelateOrLeaveEarlyDetail;//will be add some proc data
    QList<QList<QVariant> > m_lstRowLstColumnMissPunchInDetail;//will be add some proc data

    QList<QList<QVariant> > m_lstRowLstColumnCollection;//for month collection will be add some proc data

    QList<QList<QVariant> > m_lstRowLstColumnWorkDays;//for syncWorkDays

signals:
    
public slots:
    
};

#endif // FPDATAPROC_H
