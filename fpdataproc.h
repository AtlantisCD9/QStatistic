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
    //��ˢ���쳣��ʱ�������ϸ��ʱ֮ǰ��������ϸ��ʱ�в����ڵĹ�ʱ
    void addInfoIntoDutyDetailByProcAbnormalDetail();
    //�����쳣��ʱ�������ݣ�ˢ�¹�ʱ��ϸ���
    void updateDutyDetailByProcAbnormalDetail();
    //���ݹ�ʱ��ϸ��ˢ���½���ܱ������Ϣ
    void updateDutyPersonalSumByDutyDetailMemDb();
    //ˢ���½���ܱ����ֹʱ��
    void updateDutyPersonalSumSetDateInterMemDb(QDate startDate,QDate endDate);
    //����PO�л���Ϣˢ���½���ܱ����ֹʱ��
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

    QList<QVariant> m_lstTitleDetail;//��ϸ̧ͷ
    QList<QVariant> m_lstTitleCollection;//����̧ͷ

    QList<QVariant> m_lstTitleAbnormal;//�쳣�����̧ͷ
    QList<QList<QVariant> > m_lstRowLstColumnAbnormal;//�쳣�������ϸ

    QList<QVariant> m_lstTitlePOSwitch;//PO�л���̧ͷ
    QList<QList<QVariant> > m_lstRowLstColumnPOSwitch;//PO�л�����ϸ


    QList<QList<QVariant> > m_lstRowLstColumnDetail;//will be add some proc data

    QList<QList<QVariant> > m_lstRowLstColumnBelateOrLeaveEarlyDetail;//will be add some proc data
    QList<QList<QVariant> > m_lstRowLstColumnMissPunchInDetail;//will be add some proc data

    QList<QList<QVariant> > m_lstRowLstColumnCollection;//for month collection will be add some proc data

    QList<QList<QVariant> > m_lstRowLstColumnWorkDays;//for syncWorkDays

signals:
    
public slots:
    
};

#endif // FPDATAPROC_H
