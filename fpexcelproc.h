#ifndef FPEXCELPROC_H
#define FPEXCELPROC_H

#include <QObject>

class QExcel;

class FpExcelProc : public QObject
{
    Q_OBJECT
public:
    explicit FpExcelProc(QObject *parent = 0);
    ~FpExcelProc();

    bool getDataFromExcel(QList<QVariant> &lstTitle,QList<QList<QVariant> > &lstLstContent);

    QString getExcelColumnName(const int column);


    bool prepareExcel(const int sheetNum=3);
    bool saveExcel(const QString excelType="51");
    bool setDataIntoExcel(QList<QVariant> &lstTitle, QList<QList<QVariant> > &lstLstContent, const int sheetID);

private:
    QExcel *m_pExcel;
    QString m_fileSaveAsName;


};

#endif // FPEXCELPROC_H
