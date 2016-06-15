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

    //��ʼ���ڴ����ݿ���Ϣ
    bool prepareMemDb();
    //��ʼ���ı����ݿ���Ϣ
    bool prepareLocalDb();


    //��ʼ����ʱ��ϸ��
    bool initDutyDetailMemDb();
    //��ʼ���쳣��ʱ�����
    bool initProcAbnormalDetailMemDb();
    //��ʼ���½���ܱ�
    bool initDutyPersonalSumMemDb();
    //��ʼ��PO�л���
    bool initPoSwitchMemDb();

    //����ʱ��ϸ�����ڴ����ݿ�
    bool setDutyDetailIntoMemDb(QList<QList<QVariant> > &lstStrLstContent,QString &dbErrorInfo);
    //���쳣��ʱ�������ڴ����ݿ�
    bool setProcAbnormalDetailIntoMemDb(QList<QList<QVariant> > &lstStrLstContent,QString &dbErrorInfo);
    //��Po�л���Ϣ�ڴ����ݿ�
    bool setPoSwitchIntoMemDb(QList<QList<QVariant> > &lstStrLstContent,QString &dbErrorInfo);

    //ˢ�¼Ӱ๤ʱ������8Сʱ�ģ�����8Сʱ���㣬ͬʱˢ�����ֹ�ʱ
    bool updateDutyOverHours();
    //�����ϸ����쳣��ʱ�������ȡ�쳣��ʱ������Ա������Ϣ
    bool getBaseInfoInProcAbnormalDetailFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    //�����֤���룬��ȡ�쳣��ʱ������в�����ϸ��Ĺ����ʶ
    bool getTimeFlagNotInDetailInProcAbnormalDetailFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                                             const QString &POID, const QString &IDNumber);
    //�����쳣��ʱ�������ݣ�ˢ�¹�ʱ��ϸ���
    bool updateDutyDetailByProcAbnormalDetail();
    //���ݹ�ʱ��ϸ��ˢ���½���ܱ������Ϣ
    bool updateDutyPersonalSumByDutyDetailMemDb();
    //ˢ���½���ܱ����ֹʱ��
    bool updateDutyPersonalSumSetDateInterMemDb(QDate startDate,QDate endDate);
    //����PO�л���Ϣˢ���½���ܱ����ֹʱ��
    bool updateDutyPersonalSumByPoSwitchMemDb();

    //���ڴ����ݿ�ȡ���½���ܱ�
    bool getDutyPersonalSumFromMemDb(QList<QList<QVariant> > &lstStrLstContent);

    //���ù����սڼ������ñ��ڴ����ݿ�
    bool setWorkDaysIntoMemDb(QList<QList<QVariant> > &lstStrLstContent);
    //���ù����սڼ������ñ��ļ����ݿ�
    bool setWorkDaysIntoLocalDb(QList<QList<QVariant> > &lstStrLstContent);
    //���ڴ����ݿ��л�ȡ�����սڼ������ñ�����
    bool getWorkDaysFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    //���ļ����ݿ��л�ȡ�����սڼ������ñ�����
    bool getWorkDaysFromLocalDb(QList<QList<QVariant> > &lstStrLstContent);

    //���ڴ����ݿ��л�ȡ���¹����սڼ������ñ�����
    bool getWorkDaysByCurMonthFromMemDb(QList<QList<QVariant> > &lstStrLstContent,const QString &strCurMonth);
    //��ѯ�ڴ����ݿ�����С������ڣ����ڼ�鵼�뵽����Ƿ���¶�
    bool getMinMaxMonthFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    //��POID�����֤���룬��ѯ�򿨹�ʱ���쳣��ʱ������ϸ
    bool getDutyDetailByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                          const QString &POID, const QString &IDNumber);
    //��POID�����֤���룬��ѯ�������쳣��ʱ����
    bool getDutyAbnormalHoursSumByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                          const QString &POID, const QString &IDNumber);
    //��POID�����֤���룬��ѯ�����������򿨹�ʱ����
    bool getDutyPunchInHoursSumByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                          const QString &POID, const QString &IDNumber);
    //��POID�����֤���룬��ѯ�����պͽڼ��մ򿨹�ʱ����
    bool getDutyOverHoursSumByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                          const QString &POID, const QString &IDNumber);
    //���ڴ����ݿ����PO�����֤���룬��ȡ������ֹʱ��
    bool getDutyPersDateByPoIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                                const QString &POID, const QString &IDNumber);

    //��ѯ��ϸ��ʱ��Ϣ
    bool getDutyDetailFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    //��ѯ�ٵ����˹�ʱ��Ϣ
    bool getDutyDetailBelateOrLeaveEarlyFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    //��ѯ���¸ڼ�¼��ʧ��ʱ��Ϣ
    bool getDutyDetailMissPunchInFromMemDb(QList<QList<QVariant> > &lstStrLstContent);


public:
    const static QString s_strDetailSQL;
    const static QString s_strDetailBelateOrLeaveEarlySQL;
    const static QString s_strDetailMissPunchInSQL;
//    const static QString m_strCollectionSQL;

private:
    bool getWorkDaysFromDb(QList<QList<QVariant> > &lstStrLstContent,DbOper *argDbOper);
    bool setWorkDaysIntoDb(QList<QList<QVariant> > &lstStrLstContent,DbOper *argDbOper,QString &dbErrorInfo);

    //��ʼ����ʱ��ϸ��
    bool initDutyDetailDb(DbOper *argDbOper);
    //��ʼ���쳣��ʱ�����
    bool initProcAbnormalDetailDb(DbOper *argDbOper);
    //��ʼ���½���ܱ�
    bool initDutyPersonalSumDb(DbOper *argDbOper);
    //��ʼ��PO�л���
    bool initPoSwitchDb(DbOper *argDbOper);
    //��ʼ�������չ����սڼ������ñ�
    bool initDaysPayrollMultiDb(DbOper *argDbOper);
    //��ʼ��Ա����Ϣ��
    bool initEmployeeInfoDb(DbOper *argDbOper);


private:
    DbOper *m_pDbOperMem;
    DbOper *m_pDbOperLocal;

    bool m_bIsMemPrepared;//�Ƿ��ʼ����ʶ
    bool m_bIsLocalPrepared;//�Ƿ��ʼ����ʶ��

};

#endif // FPDBPROC_H
