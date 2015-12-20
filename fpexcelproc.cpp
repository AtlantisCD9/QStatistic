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

bool FpExcelProc::getExcelOpenFile(QString &fileName)
{
    fileName = QFileDialog::getOpenFileName(0,
                                            tr("Import Excel"),
                                            QString(),
                                            tr("Excel Files (*.xls *.xlsx)"));

    if (fileName.isNull())
    {
        //user press cancel
        return false;
    }
    return true;
}

bool FpExcelProc::getExcelOpenFileList(QStringList &lstFileName)
{
    lstFileName = QFileDialog::getOpenFileNames(0,
                                                tr("Merge Excel"),
                                                QString(),
                                                tr("Excel Files (*.xls *.xlsx)"));

    if (lstFileName.isEmpty())
    {
        //user press cancel
        return false;
    }
    return true;
}

bool FpExcelProc::getDataFromExcel(const QString fileName,
                                   QList<QVariant> &lstTitle, QList<QList<QVariant> > &lstLstContent, const int sheetID)
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

    if (256 == bottomRightColumn)
    {
        bottomRightColumn = 12;//特殊处理
    }

    QString rangeCell;
    rangeCell += getExcelColumnName(topLeftColumn);
    rangeCell += QString("%1");
    rangeCell.append(":");
    rangeCell += getExcelColumnName(bottomRightColumn);
    rangeCell += QString("%2");

    //qDebug() << rangeCell.arg(topLeftRow).arg(bottomRightRow);

    //get title
    QVariant titleExcel = m_pExcel->getCellValue(rangeCell.arg(topLeftRow).arg(topLeftRow));
    lstTitle = titleExcel.toList().first().toList();

    ++topLeftRow;

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

bool FpExcelProc::getExcelSaveFile(QString &fileName)
{
    //prepare for save as file name
    fileName = QFileDialog::getSaveFileName(0,
                                          tr("Export Excel"),
                                          QString(),
                                          tr("Excel Files (*.xlsx *.xls)"));

    if (fileName.isNull())
    {
        //user press cancel;
        return false;
    }
    fileName = QDir::toNativeSeparators(fileName);
    return true;
}

bool FpExcelProc::prepareExcel(ENUM_XLS_TYPE xlsType, const int sheetNum)
{
    //load source xls
    QString sourceFileName;
    switch (xlsType)
    {
    case MONTH_TOTAL:
        sourceFileName = "month_total.xlsx";
    case MERGE_TOTAL:
        sourceFileName = "merge_total.xlsx";
    default:
        sourceFileName = "month_total.xlsx";
    }

    sourceFileName = QDir::currentPath()+"./xlsSource/"+sourceFileName;
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

bool FpExcelProc::setDataIntoExcel(QList<QVariant> &lstTitle, QList<QList<QVariant> > &lstLstContent, const int sheetID)
{
    if (NULL == m_pExcel)
    {
        return false;
    }

    //取得第一个工作表
    m_pExcel->selectSheet(sheetID);

    //工作表内容范围
    int topLeftRow, topLeftColumn, bottomRightRow, bottomRightColumn;
    topLeftRow = 2;
    topLeftColumn = 1;
    bottomRightRow = lstLstContent.size()+topLeftRow-1;
    bottomRightColumn = lstTitle.size()+topLeftColumn-1;

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
    m_pExcel->setCellVariant(rangeCell.arg(1).arg(1), QVariant(QVariantList(lstTitle)));

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

//    qDebug() << lstTitle;
//    foreach(QList<QVariant> strLstContent,lstLstContent)
//    {
//        qDebug() << strLstContent;
//    }
//    qDebug() << lstLstContent.size();



//    qDebug() << topLeftRow;
//    qDebug() << topLeftColumn;
//    qDebug() << bottomRightRow;
//    qDebug() << bottomRightColumn;
    //m_pExcel->getCellValue(2, 2).toString();
    //删除工作表
    //m_pExcel->selectSheet("Sheet1");
    //m_pExcel->selectSheet(1);
    //m_pExcel->deleteSheet();
    //m_pExcel->save();
    //插入数据
    //m_pExcel->selectSheet("qExcelSheet3");
    //m_pExcel->setCellString(1, 7, "addString");
    //m_pExcel->setCellString("A3", "abc");
    //m_pExcel->save();
    //合并单元格
    //m_pExcel->selectSheet(2);
    //m_pExcel->mergeCells("G1:H2");
    //m_pExcel->mergeCells(4, 7, 5 ,8);
    //m_pExcel->save();
    //设置列宽
    //m_pExcel->selectSheet(1);
    //m_pExcel->setColumnWidth(1, 20);
    //m_pExcel->save();
    //设置粗体
    //m_pExcel->selectSheet(1);
    //m_pExcel->setCellFontBold(2, 2, true);
    //m_pExcel->setCellFontBold("A2", true);
    //m_pExcel->save();
    //设置文字大小
    //m_pExcel->selectSheet(1);
    //m_pExcel->setCellFontSize("B3", 20);
    //m_pExcel->setCellFontSize(1, 2, 20);
    //m_pExcel->save();
    //设置单元格文字居中
    //m_pExcel->selectSheet(2);
    //m_pExcel->setCellTextCenter(1, 2);
    //m_pExcel->setCellTextCenter("A2");
    //m_pExcel->save();
    //设置单元格文字自动折行
    //m_pExcel->selectSheet(1);
    //m_pExcel->setCellTextWrap(2,2,true);
    //m_pExcel->setCellTextWrap("A2", true);
    //m_pExcel->save();
    //设置一行自适应行高
    //m_pExcel->selectSheet(1);
    //m_pExcel->setAutoFitRow(2);
    //m_pExcel->save();
    //新建工作表
    //m_pExcel->insertSheet("abc");
    //m_pExcel->save();
    //清除单元格内容
    //m_pExcel->selectSheet(4);
    //m_pExcel->clearCell(1,1);
    //m_pExcel->clearCell("A2");
    //m_pExcel->save();
    //合并一列中相同连续的单元格
    //m_pExcel->selectSheet(1);
    //m_pExcel->mergeSerialSameCellsInColumn(1, 2);
    //m_pExcel->save();
    //获取一张工作表已用行数
    //m_pExcel->selectSheet(1);
    //qDebug()<<m_pExcel->getUsedRowsCount();
    //设置行高
//    m_pExcel->selectSheet(1);
//    m_pExcel->setRowHeight(2, 30);
//    pExcel->save();
}
