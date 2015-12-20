#ifndef FPDATAPROC_H
#define FPDATAPROC_H

#include <QObject>
#include <QDateTime>

class FpDbProc;
class FpExcelProc;

class FpDataProc : public QObject
{
    Q_OBJECT
public:
    explicit FpDataProc(QObject *parent = 0);
    ~FpDataProc();

    void initial();

    //Data proc
    void procDataForDatail();
    void procDataForCollection();

    //Excel proc
    bool getDataFromExcel();
    void setDataIntoExcel();
    void mergeExcel();

    //Loacl Db

    //Mem Db
    void setDutyDetail();
    const QList<QVariant> &getDutyDetailTitle(){return m_lstTitleDetail;}
    const QString &getDutyDetailSQL();
    const QString &getDutyDetailBelateOrLeaveEarlySQL();
    const QString &getDutyDetailMissPunchInSQL();

    void getBelateOrLeaveEarlyDetail();
    void getMissPunchInDetail();
    void getDistinctPersonal();

    //void setDutyCollection();
    const QList<QVariant> &getDutyColletionTitle(){return m_lstTitleCollection;}
    const QList<QList<QVariant> > &getDutyColletionLstRowLstColumn(){return m_lstRowLstColumnCollection;}
//    const QString &getDutyColletionSQL();

    void syncWorkDays();

private:
    //Loacl Db
    void getWorkDays();
    void createWorkDays();

    bool getAndCheckCurMonth();//example:201510

    int getAbnormalHours(QDateTime dtTimeFlag,QDateTime dtStart,QDateTime dtEnd);
    int getPunchInHours(QDateTime dtTimeFlag,QDateTime dtStart,QDateTime dtEnd);
    int getOverTimeHours(QDateTime dtTimeFlag,QDateTime dtStart,QDateTime dtEnd);
    
private:
    FpDbProc *m_pFpDbProc;
    FpExcelProc *m_pFpExcelProc;
    QString m_strDateMonth;//format:201510

    QList<QVariant> m_lstTitleDetail;//明细抬头
    QList<QVariant> m_lstTitleCollection;//汇总抬头

    QList<QList<QVariant> > m_lstRowLstColumnDetail;//will be add some proc data

    QList<QList<QVariant> > m_lstRowLstColumnBelateOrLeaveEarlyDetail;//will be add some proc data
    QList<QList<QVariant> > m_lstRowLstColumnMissPunchInDetail;//will be add some proc data

    QList<QList<QVariant> > m_lstRowLstColumnCollection;//for month collection will be add some proc data

    QList<QList<QVariant> > m_lstRowLstColumnWorkDays;//for syncWorkDays

signals:
    
public slots:
    
};

#endif // FPDATAPROC_H
