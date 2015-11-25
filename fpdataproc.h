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

private:
    //Loacl Db
    void getWorkDays();
    void setWorkDays();
    
private:
    FpDbProc *m_pFpDbProc;
    FpExcelProc *m_pFpExcelProc;

    QList<QVariant> m_lstTitleExcel;
    QList<QList<QVariant> > m_lstLstContentExcel;

    QList<QList<QVariant> > m_lstLstContentWorkDays;

    QList<QList<QVariant> > m_lstLstContentDutyCollection;

signals:
    
public slots:
    
};

#endif // FPDATAPROC_H
