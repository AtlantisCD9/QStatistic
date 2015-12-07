#ifndef FPEXCELPROC_H
#define FPEXCELPROC_H

#include <QObject>

class FpExcelProc : public QObject
{
    Q_OBJECT
public:
    explicit FpExcelProc(QObject *parent = 0);
    ~FpExcelProc();

    bool getDataFromExcel(QList<QVariant> &lstTitle,QList<QList<QVariant> > &lstLstContent);
    bool setDataIntoExcel(QList<QVariant> &lstTitle, QList<QList<QVariant> > &lstLstContent, const int sheetID=1);

};

#endif // FPEXCELPROC_H
