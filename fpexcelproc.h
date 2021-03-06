#ifndef FPEXCELPROC_H
#define FPEXCELPROC_H

#include "globaldef.h"

#include <QObject>

class QExcel;

class FpExcelProc : public QObject
{
    Q_OBJECT
public:
    explicit FpExcelProc(QObject *parent = 0);
    ~FpExcelProc();

//    bool getExcelOpenFile(QString &fileName);
//    bool getExcelOpenFileList(QStringList &lstFileName);
    bool getDataFromExcel(const QString fileName,
                          QList<QVariant> &lstTitle, QList<QList<QVariant> > &lstLstContent,
                          const int titleEndID=1, const int sheetID=1, const int columnNum=0);

    QString getExcelColumnName(const int column);


//    bool getExcelSaveFile(QString &fileName);
    bool protectSourceFile(const QString outPutFile);
    bool prepareExcel(ENUM_EXPORT_XLS_TYPE xlsType,const int sheetNum=3);
    bool setDataIntoExcel(QList<QVariant> &lstTitle, QList<QList<QVariant> > &lstLstContent,
                          const int titleEndID=1,const int sheetID=1);
    bool saveExcel(const QString &fileName,const QString excelType="51");

private:
    QExcel *m_pExcel;


};

#endif // FPEXCELPROC_H
