#ifndef FPEXCELPROC_H
#define FPEXCELPROC_H

#include <QObject>

class FpExcelProc : public QObject
{
    Q_OBJECT
public:
    ~FpExcelProc();
    static FpExcelProc *getInstance(QObject *parent = 0);
    static void releaseInstance();

    void procData();
    bool getDataFromExcel();
    bool setDataIntoExcel();

private:
    explicit FpExcelProc(QObject *parent = 0);


private:
    static FpExcelProc *m_pInstance;

    QList<QVariant> m_strLstTitleContent;
    QList<QList<QVariant> > m_lstStrLstContent;
    
signals:
    
public slots:
    
};

#endif // FPEXCELPROC_H
