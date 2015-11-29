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
    void procData();

    //Excel proc
    void getDataFromExcel();
    void setDataIntoExcel();

    //Loacl Db

    //Mem Db
    void getDutyCollection();
    void setDutyDetail();

    void syncWorkDays();

private:
    //Loacl Db
    void getWorkDays();
    void setWorkDays();
    
private:
    FpDbProc *m_pFpDbProc;
    FpExcelProc *m_pFpExcelProc;

    QList<QVariant> m_lstTitleDetail;
    QList<QList<QVariant> > m_lstRowLstColumnDetail;

    //��ȱʧ��¼
    QList<QVariant> m_lstTitleDetailMissPunchIn;
    QList<QList<QVariant> > m_lstRowLstColumnDetailMissPunchIn;

    //�ٵ�����
    QList<QVariant> m_lstTitleDetailBelateOrleaveEarly;
    QList<QList<QVariant> > m_lstRowLstColumnDetailBelateOrleaveEarly;

    QList<QList<QVariant> > m_lstRowLstColumnWorkDays;

    QList<QList<QVariant> > m_lstRowLstColumnDutyCollection;

signals:
    
public slots:
    
};

#endif // FPDATAPROC_H
