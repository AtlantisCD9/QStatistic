#include "fpexcelproc.h"
#include "qexcel.h"

#include <QDateTime>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDir>


const int SheetID = 1;
const int MaxRow = 100;
const int OnDutyID = 7;
const int OffDutyID = 8;


FpExcelProc *FpExcelProc::m_pInstance = NULL;

FpExcelProc *FpExcelProc::getInstance(QObject *parent)
{
    if(m_pInstance == NULL)//判断是否第一次调用
    {
        m_pInstance = new FpExcelProc(parent);
    }
    return m_pInstance;
}

void FpExcelProc::releaseInstance()
{
    if(NULL != m_pInstance)//判断是否已经释放
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}


FpExcelProc::FpExcelProc(QObject *parent) :
    QObject(parent)
{
}

FpExcelProc::~FpExcelProc()
{
    QExcel::releaseInstance();
}

void FpExcelProc::procData()
{
    for(int i=0;i<m_lstStrLstContent.size();++i)
    {
        QDateTime dtTimeFlag;
        QDateTime dtStart = m_lstStrLstContent[i][OnDutyID].toDateTime();
        QDateTime dtEnd = m_lstStrLstContent[i][OffDutyID].toDateTime();
        if (dtStart.time() >= QTime(5,1))
        {
            dtTimeFlag = QDateTime(dtStart.date(),QTime(0,0));
        }
        else
        {
            dtTimeFlag = QDateTime(dtStart.date().addDays(-1),QTime(0,0));
        }

        //add timeflag
        m_lstStrLstContent[i] << QVariant(dtTimeFlag);

        //add total hour
        if (!dtEnd.isNull())
        {
            m_lstStrLstContent[i] << QVariant(dtStart.secsTo(dtEnd)*1.0/3600);
        }
        else
        {
            m_lstStrLstContent[i] << QVariant(0.0);
        }

        //day of week
        m_lstStrLstContent[i] << QVariant(dtTimeFlag.date().dayOfWeek());

        //abnormal work hours:0,normal;1,abnormal;2,beLate or leaveEarly
        if (dtEnd.isNull())
        {
            m_lstStrLstContent[i] << QVariant(1);
        }
        else
        {
            if(dtStart >= QDateTime(dtTimeFlag.date(),QTime(9,1))
                    || dtEnd < QDateTime(dtTimeFlag.date(),QTime(17,30)))
            {
                m_lstStrLstContent[i] << QVariant(2);
            }
            else
            {
                m_lstStrLstContent[i] << QVariant(0);//normal
            }
        }
        //qDebug() << m_lstStrLstContent[i];
    }
}

bool FpExcelProc::getDataFromExcel()
{
    QString fileName = QFileDialog::getOpenFileName(0,
                                                    tr("Import Excel"),
                                                    QString(),
                                                    tr("Excel Files (*.xls *.xlsx)"));
    if (fileName.isNull())
    {
        //user press cancel
        return false;
    }

    if (!QFile::exists(fileName))
    {
        QMessageBox::critical(0,"Error",QString("The File Does Not Exist:%1").arg(fileName));
        return false;
    }

    QExcel::releaseInstance();
    QExcel *pExcel = QExcel::getInstance(fileName);
    if (!pExcel)
    {
        qDebug() << "Get Excel API Static Object Failed";
        return false;
    }

    //取得工作表数量
    if (pExcel->getSheetsCount() < 1)
    {
        QMessageBox::critical(0,"Error","Sheets Is Empty!");
        return false;
    }
    //取得第一个工作表
    pExcel->selectSheet(SheetID);
    //取得工作表已使用范围
    int topLeftRow, topLeftColumn, bottomRightRow, bottomRightColumn;
    pExcel->getUsedRange(&topLeftRow, &topLeftColumn, &bottomRightRow, &bottomRightColumn);

    //get title
    for (int i=topLeftRow,j=topLeftColumn;j<=bottomRightColumn;++j)
    {
        m_strLstTitleContent << pExcel->getCellValue(i,j);
    }
    ++topLeftRow;

    //just for debug
    if (-1 !=MaxRow && MaxRow < bottomRightRow)
    {
        bottomRightRow = MaxRow;
    }


    QProgressDialog progress("Loading data...", "Abort Load", 0, bottomRightRow+1-topLeftRow, 0);
         progress.setWindowModality(Qt::WindowModal);

    for (int i=topLeftRow;i<=bottomRightRow;++i)
    {
        m_lstStrLstContent << QList<QVariant>();
        for (int j=topLeftColumn;j<=bottomRightColumn;++j)
        {
            m_lstStrLstContent.last() << pExcel->getCellValue(i,j);
        }
        progress.setValue(i-topLeftRow);
        if (progress.wasCanceled())
        {
            return true;
        }
    }
    progress.setValue(bottomRightRow+1-topLeftRow);

    return true;

//    qDebug() << m_strLstTitleContent;
//    foreach(QList<QVariant> strLstContent,m_lstStrLstContent)
//    {
//        qDebug() << strLstContent;
//    }
//    qDebug() << m_lstStrLstContent.size();
}

bool FpExcelProc::setDataIntoExcel()
{
    QString fileName = QFileDialog::getSaveFileName(0,
                                                    tr("Export Excel"),
                                                    QString(),
                                                    tr("Excel Files (*.xlsx *.xls)"));
    if (fileName.isNull())
    {
        //user press cancel;
        return false;
    }

    if (0 == m_lstStrLstContent.size())
    {
        QMessageBox::information(0,"info","没有需要到处的数据");
        return false;
    }

    QString sourceFileName;
    sourceFileName = QDir::currentPath()+"./xlsSource/month_total.xlsx";
    sourceFileName = QDir::toNativeSeparators(QDir::cleanPath(fileName));

    if (!QFile::exists(sourceFileName))
    {
        QMessageBox::critical(0,"Error",QString("The File Does Not Exist:%1").arg(sourceFileName));
        return false;
    }

    QExcel::releaseInstance();
    QExcel *pExcel = QExcel::getInstance(fileName);
    if (!pExcel)
    {
        qDebug() << "Get Excel API Static Object Failed";
        return false;
    }

    //取得工作表数量
    if (pExcel->getSheetsCount() < 1)
    {
        QMessageBox::critical(0,"Error","Sheets Is Empty!");
        return false;
    }
    //取得第一个工作表
    pExcel->selectSheet(SheetID);
    //取得工作表已使用范围
    int topLeftRow, topLeftColumn, bottomRightRow, bottomRightColumn;
    topLeftRow = 5;
    topLeftColumn = 0;
    bottomRightRow = m_lstStrLstContent.size();
    bottomRightColumn = m_strLstTitleContent.size();

    QProgressDialog progress("Exporting data...", "Abort Export", 0, bottomRightRow+1-topLeftRow, 0);
         progress.setWindowModality(Qt::WindowModal);

    for (int i=0;i<=bottomRightRow-topLeftRow;++i)
    {
        for (int j=0;j<=bottomRightColumn-topLeftColumn;++j)
        {
            pExcel->setCellVariant(i+topLeftRow, j+topLeftColumn, m_lstStrLstContent[i][j]);
        }
        progress.setValue(i-topLeftRow);
        if (progress.wasCanceled())
        {
            return true;
        }
    }
    progress.setValue(bottomRightRow+1-topLeftRow);

    fileName = QDir::toNativeSeparators(fileName);
    pExcel->saveAs(fileName,"51");

    return true;

//    qDebug() << m_strLstTitleContent;
//    foreach(QList<QVariant> strLstContent,m_lstStrLstContent)
//    {
//        qDebug() << strLstContent;
//    }
//    qDebug() << m_lstStrLstContent.size();



//    qDebug() << topLeftRow;
//    qDebug() << topLeftColumn;
//    qDebug() << bottomRightRow;
//    qDebug() << bottomRightColumn;
    //pExcel->getCellValue(2, 2).toString();
    //删除工作表
    //pExcel->selectSheet("Sheet1");
    //pExcel->selectSheet(1);
    //pExcel->deleteSheet();
    //pExcel->save();
    //插入数据
    //pExcel->selectSheet("qExcelSheet3");
    //pExcel->setCellString(1, 7, "addString");
    //pExcel->setCellString("A3", "abc");
    //pExcel->save();
    //合并单元格
    //pExcel->selectSheet(2);
    //pExcel->mergeCells("G1:H2");
    //pExcel->mergeCells(4, 7, 5 ,8);
    //pExcel->save();
    //设置列宽
    //pExcel->selectSheet(1);
    //pExcel->setColumnWidth(1, 20);
    //pExcel->save();
    //设置粗体
    //pExcel->selectSheet(1);
    //pExcel->setCellFontBold(2, 2, true);
    //pExcel->setCellFontBold("A2", true);
    //pExcel->save();
    //设置文字大小
    //pExcel->selectSheet(1);
    //pExcel->setCellFontSize("B3", 20);
    //pExcel->setCellFontSize(1, 2, 20);
    //pExcel->save();
    //设置单元格文字居中
    //pExcel->selectSheet(2);
    //pExcel->setCellTextCenter(1, 2);
    //pExcel->setCellTextCenter("A2");
    //pExcel->save();
    //设置单元格文字自动折行
    //pExcel->selectSheet(1);
    //pExcel->setCellTextWrap(2,2,true);
    //pExcel->setCellTextWrap("A2", true);
    //pExcel->save();
    //设置一行自适应行高
    //pExcel->selectSheet(1);
    //pExcel->setAutoFitRow(2);
    //pExcel->save();
    //新建工作表
    //pExcel->insertSheet("abc");
    //pExcel->save();
    //清除单元格内容
    //pExcel->selectSheet(4);
    //pExcel->clearCell(1,1);
    //pExcel->clearCell("A2");
    //pExcel->save();
    //合并一列中相同连续的单元格
    //pExcel->selectSheet(1);
    //pExcel->mergeSerialSameCellsInColumn(1, 2);
    //pExcel->save();
    //获取一张工作表已用行数
    //pExcel->selectSheet(1);
    //qDebug()<<pExcel->getUsedRowsCount();
    //设置行高
//    pExcel->selectSheet(1);
//    pExcel->setRowHeight(2, 30);
//    pExcel->save();
}
