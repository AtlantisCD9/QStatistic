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

    void procData();
    void getDataFromExcel();
    void setDataIntoExcel();

    void prepareDb();
    void getDataFromDb();
    void setDataIntoDb();
    
private:
    FpDbProc *m_pFpDbProc;
    FpExcelProc *m_pFpExcelProc;

    QList<QVariant> m_lstTitle;
    QList<QList<QVariant> > m_lstLstContent;

    QList<QList<QVariant> > m_lstLstContentDb;

signals:
    
public slots:
    
};

#endif // FPDATAPROC_H
