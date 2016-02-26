#include "fpexcelproc.h"
#include "qexcel.h"

#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDir>

const int MaxRow = -1;

FpExcelProc::FpExcelProc(QObject *parent) :
    QObject(parent),
    m_pExcel(NULL)
{
}

FpExcelProc::~FpExcelProc()
{
    if (NULL != m_pExcel)
    {
        delete m_pExcel;
        m_pExcel = NULL;
    }
}

//bool FpExcelProc::getExcelOpenFile(QString &fileName)
//{
//    fileName = QFileDialog::getOpenFileName(0,
//                                            tr("Import Excel"),
//                                            QString(),
//                                            tr("Excel Files (*.xls *.xlsx)"));

//    if (fileName.isNull())
//    {
//        //user press cancel
//        return false;
//    }
//    return true;
//}

//bool FpExcelProc::getExcelOpenFileList(QStringList &lstFileName)
//{
//    lstFileName = QFileDialog::getOpenFileNames(0,
//                                                tr("Merge Excel"),
//                                                QString(),
//                                                tr("Excel Files (*.xls *.xlsx)"));

//    if (lstFileName.isEmpty())
//    {
//        //user press cancel
//        return false;
//    }
//    return true;
//}

bool FpExcelProc::getDataFromExcel(const QString fileName,
                                   QList<QVariant> &lstTitle, QList<QList<QVariant> > &lstLstContent,
                                   const int titleEndID, const int sheetID, const int columnNum)
{
    if (!QFile::exists(fileName))
    {
        QMessageBox::critical(0,"Error",QString("The File Does Not Exist:%1").arg(fileName));
        return false;
    }

    m_pExcel = new QExcel(fileName);
    if (!m_pExcel)
    {
        qDebug() << "Get Excel API Static Object Failed";
        return false;
    }

    //取得工作表数量
    if (m_pExcel->getSheetsCount() < 1)
    {
        QMessageBox::critical(0,"Error","Sheets Is Empty!");
        delete m_pExcel;
        m_pExcel = NULL;
        return false;
    }
    //取得第一个工作表
    m_pExcel->selectSheet(sheetID);
    //取得工作表已使用范围
    int topLeftRow, topLeftColumn, bottomRightRow, bottomRightColumn;
    m_pExcel->getUsedRange(&topLeftRow, &topLeftColumn, &bottomRightRow, &bottomRightColumn);

    if(0 == columnNum)
    {
        if (256 == bottomRightColumn)
        {
            bottomRightColumn = 12;//特殊处理
        }
    }
    else
    {
        bottomRightColumn = columnNum;
    }

    QString rangeCell;
    rangeCell += getExcelColumnName(topLeftColumn);
    rangeCell += QString("%1");
    rangeCell.append(":");
    rangeCell += getExcelColumnName(bottomRightColumn);
    rangeCell += QString("%2");

    //qDebug() << rangeCell.arg(topLeftRow).arg(bottomRightRow);

    //get title
    QVariant titleExcel = m_pExcel->getCellValue(rangeCell.arg(topLeftRow).arg(topLeftRow+titleEndID-1));
    if (1 ==titleEndID)
    {
        lstTitle = titleExcel.toList().first().toList();
    }
    else
    {
        lstTitle = QList<QVariant>();
    }


    //++topLeftRow;
    topLeftRow += titleEndID;

    //just for debug
    if (-1 !=MaxRow && MaxRow < bottomRightRow)
    {
        bottomRightRow = MaxRow;
    }

    //get content
    QVariant contentExcel = m_pExcel->getCellValue(rangeCell.arg(topLeftRow).arg(bottomRightRow));

    QProgressDialog progress("Loading data...", "Abort Load", 0, bottomRightRow+1-topLeftRow, 0);
         progress.setWindowModality(Qt::WindowModal);

    for (int i=0;i<bottomRightRow+1-topLeftRow;++i)
    {
        if(contentExcel.toList()[i].toList().first().toString().isEmpty())
        {
		    //if some row first column is empty than break
            break;
        }
        lstLstContent << QList<QVariant>();

        lstLstContent.last() = contentExcel.toList()[i].toList();

        progress.setValue(i);
        if (progress.wasCanceled())
        {
            delete m_pExcel;
            m_pExcel = NULL;
            return true;
        }
    }
    progress.setValue(bottomRightRow+1-topLeftRow);

    delete m_pExcel;
    m_pExcel = NULL;

    //QMessageBox::information(0,"Info",QString("Import Done: %1 ").arg(bottomRightRow+1-topLeftRow));

    return true;

//    qDebug() << lstTitle;
//    foreach(QList<QVariant> strLstContent,lstLstContent)
//    {
//        qDebug() << strLstContent;
//    }
//    qDebug() << lstLstContent.size();
}

//bool FpExcelProc::getExcelSaveFile(QString &fileName)
//{
//    //prepare for save as file name
//    fileName = QFileDialog::getSaveFileName(0,
//                                          tr("Export Excel"),
//                                          QString(),
//                                          tr("Excel Files (*.xlsx *.xls)"));

//    if (fileName.isNull())
//    {
//        //user press cancel;
//        return false;
//    }
//    fileName = QDir::toNativeSeparators(fileName);
//    return true;
//}

bool FpExcelProc::protectSourceFile(const QString outPutFile)
{
    //load source xls
    QStringList lstSourceFileName;
    lstSourceFileName << "month_total.xlsx";
    lstSourceFileName << "merge_total.xlsx";

    foreach (QString sourceFileName,lstSourceFileName)
    {
        sourceFileName = QDir::currentPath()+"/xlsSource/"+sourceFileName;
        sourceFileName = QDir::toNativeSeparators(QDir::cleanPath(sourceFileName));

//        qDebug() << sourceFileName;
//        qDebug() << QDir::toNativeSeparators(QDir::cleanPath(outPutFile));

        if (QDir::toNativeSeparators(QDir::cleanPath(outPutFile)) == sourceFileName)
        {
            QMessageBox::critical(0,"Error",QString("Can Not Select The Base Excel To Save:%1\nPlease Set Another File Name").arg(sourceFileName));
            return false;
        }
    }

    return true;
}

bool FpExcelProc::prepareExcel(ENUM_EXPORT_XLS_TYPE xlsType, const int sheetNum)
{
    //load source xls
    QString sourceFileName;
    switch (xlsType)
    {
    case EX_MONTH_TOTAL:
        sourceFileName = "month_total.xlsx";
    case EX_MERGE_TOTAL:
        sourceFileName = "merge_total.xlsx";
    default:
        sourceFileName = "month_total.xlsx";
    }

    sourceFileName = QDir::currentPath()+"/xlsSource/"+sourceFileName;
    sourceFileName = QDir::toNativeSeparators(QDir::cleanPath(sourceFileName));

    if (!QFile::exists(sourceFileName))
    {
        QMessageBox::critical(0,"Error",QString("The File XlsSource Does Not Exist:%1").arg(sourceFileName));
        return false;
    }

    m_pExcel = new QExcel(sourceFileName);
    if (!m_pExcel)
    {
        qDebug() << "Get Excel API Static Object Failed";
        return false;
    }

    //取得工作表数量
    if (m_pExcel->getSheetsCount() < sheetNum)
    {
        QMessageBox::critical(0,"Error","Sheets Is Empty!");
        delete m_pExcel;
        m_pExcel = NULL;
        return false;
    }

    return true;
}

bool FpExcelProc::saveExcel(const QString &fileName, const QString excelType)
{
    if (NULL == m_pExcel)
    {
        return false;
    }

    m_pExcel->saveAs(fileName,excelType);

    delete m_pExcel;
    m_pExcel = NULL;

    QMessageBox::information(0,"Info",QString("Export Done!"));

    return true;
}

QString FpExcelProc::getExcelColumnName(const int column)
{
    QString retColumnName;
    if (column/26 >= 26)
    {
        retColumnName = "ZZ";
    }
    else if(column/26 > 0)
    {
        retColumnName.append(QChar(column/26 - 1 + 'A'));
        retColumnName.append(QChar(column%26 - 1 + 'A'));
    }
    else
    {
        retColumnName.append(QChar(column - 1 + 'A'));
    }

    return retColumnName;
}

bool FpExcelProc::setDataIntoExcel(QList<QVariant> &lstTitle, QList<QList<QVariant> > &lstLstContent,
                                   const int titleEndID, const int sheetID)
{
    if (NULL == m_pExcel)
    {
        return false;
    }

    //取得第一个工作表
    m_pExcel->selectSheet(sheetID);

    //工作表内容范围
    int topLeftRow, topLeftColumn, bottomRightRow, bottomRightColumn;
    topLeftRow = titleEndID + 1;
    topLeftColumn = 1;
    bottomRightRow = lstLstContent.size()+topLeftRow-1;
    bottomRightColumn = lstLstContent.first().size()+topLeftColumn-1;

    QString rangeCell;
    rangeCell += getExcelColumnName(topLeftColumn);
    rangeCell += QString("%1");
    rangeCell.append(":");
    rangeCell += getExcelColumnName(bottomRightColumn);
    rangeCell += QString("%2");

    //工作表抬头范围
//    for (int i=0;i<lstTitle.size();++i)
//    {
//        m_pExcel->setCellVariant(1, i+1, lstTitle[i]);
//    }
    m_pExcel->setCellVariant(rangeCell.arg(1).arg(titleEndID), QVariant(QVariantList(lstTitle)));

//    m_pExcel->setCellVariant(rangeCell.arg(topLeftRow).arg(bottomRightRow), QVariant(lstLstContent));


    QProgressDialog progress("Exporting data...", "Abort Export", 0, bottomRightRow-topLeftRow, 0);
         progress.setWindowModality(Qt::WindowModal);

    for (int i=0;i<bottomRightRow-topLeftRow+1;++i)
    {
//        for (int j=0;j<bottomRightColumn-topLeftColumn+1;++j)
//        {
//            m_pExcel->setCellVariant(i+topLeftRow, j+topLeftColumn, lstLstContent[i][j]);
//        }
        m_pExcel->setCellVariant(rangeCell.arg(i+topLeftRow).arg(i+topLeftRow), QVariant(QVariantList(lstLstContent[i])));
        progress.setValue(i);
        if (progress.wasCanceled())
        {
            delete m_pExcel;
            m_pExcel = NULL;
            return true;
        }
    }
    progress.setValue(bottomRightRow-topLeftRow+1);
//    QMessageBox::information(0,"Info",QString("Export Done: %1 ").arg(bottomRightRow+1-topLeftRow));



    return true;
}
