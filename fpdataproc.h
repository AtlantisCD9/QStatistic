#ifndef FPDATAPROC_H
#define FPDATAPROC_H

#include <QObject>

class FpDbProc;
class FpExcelProc;

class FpDataProc : public QObject
{
    Q_OBJECT
public:
    explicit FpDataProc(QObject *parent = 0);
    ~FpDataProc();

    //Data proc
    void procDataForDatail();
    void procDataForCollection();

    //Excel proc
    void getDataFromExcel();
    void setDataIntoExcel();

    //Loacl Db

    //Mem Db
    void setDutyDetail();
    const QList<QVariant> &getDutyDetailTitle(){return m_lstTitleDetail;}
    const QString &getDutyDetailSQL();
    const QString &getDutyDetailBelateOrLeaveEarlySQL();
    const QString &getDutyDetailMissPunchInSQL();


    void getDistinctPersonal();

    void setDutyCollection();
    const QList<QVariant> &getDutyColletionTitle(){return m_lstTitleCollection;}
    const QString &getDutyColletionSQL();

    void syncWorkDays();

private:
    //Loacl Db
    void getWorkDays();
    void createWorkDays();

    bool getAndCheckCurMonth();//example:201510
    
private:
    FpDbProc *m_pFpDbProc;
    FpExcelProc *m_pFpExcelProc;
    QString m_strDateMonth;//format:201510

    QList<QVariant> m_lstTitleDetail;//明细抬头
    QList<QVariant> m_lstTitleCollection;//汇总抬头

    QList<QList<QVariant> > m_lstRowLstColumnDetail;//will be add some proc data

    QList<QList<QVariant> > m_lstRowLstColumnCollection;//for month collection will be add some proc data

    QList<QList<QVariant> > m_lstRowLstColumnWorkDays;//for syncWorkDays

signals:
    
public slots:
    
};

#endif // FPDATAPROC_H
