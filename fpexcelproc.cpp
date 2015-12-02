#include "fpexcelproc.h"
#include "qexcel.h"

#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDir>


const int SheetID = 1;
const int MaxRow = -1;


FpExcelProc::FpExcelProc(QObject *parent) :
    QObject(parent)
{
}

FpExcelProc::~FpExcelProc()
{
}

bool FpExcelProc::getDataFromExcel(QList<QVariant> &lstTitle, QList<QList<QVariant> > &lstLstContent)
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

    QExcel *pExcel = new QExcel(fileName);
    if (!pExcel)
    {
        qDebug() << "Get Excel API Static Object Failed";
        return false;
    }

    //ȡ�ù���������
    if (pExcel->getSheetsCount() < 1)
    {
        QMessageBox::critical(0,"Error","Sheets Is Empty!");
        delete pExcel;
        return false;
    }
    //ȡ�õ�һ��������
    pExcel->selectSheet(SheetID);
    //ȡ�ù�������ʹ�÷�Χ
    int topLeftRow, topLeftColumn, bottomRightRow, bottomRightColumn;
    pExcel->getUsedRange(&topLeftRow, &topLeftColumn, &bottomRightRow, &bottomRightColumn);

    //get title
    for (int i=topLeftRow,j=topLeftColumn;j<=bottomRightColumn;++j)
    {
        lstTitle << pExcel->getCellValue(i,j);
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
        lstLstContent << QList<QVariant>();
        for (int j=topLeftColumn;j<=bottomRightColumn;++j)
        {
            lstLstContent.last() << pExcel->getCellValue(i,j);
        }
        progress.setValue(i-topLeftRow);
        if (progress.wasCanceled())
        {
            delete pExcel;
            return true;
        }
    }
    progress.setValue(bottomRightRow+1-topLeftRow);

    delete pExcel;

    QMessageBox::information(0,"Info",QString("Import Done: %1 ").arg(bottomRightRow+1-topLeftRow));

    return true;

//    qDebug() << lstTitle;
//    foreach(QList<QVariant> strLstContent,lstLstContent)
//    {
//        qDebug() << strLstContent;
//    }
//    qDebug() << lstLstContent.size();
}

bool FpExcelProc::setDataIntoExcel(QList<QVariant> &lstTitle, QList<QList<QVariant> > &lstLstContent)
{
    if (0 == lstLstContent.size())
    {
        return true;
    }

    //prepare for save as file name
    QString fileSaveAsName = QFileDialog::getSaveFileName(0,
                                                          tr("Export Excel"),
                                                          QString(),
                                                          tr("Excel Files (*.xlsx *.xls)"));
    if (fileSaveAsName.isNull())
    {
        //user press cancel;
        return false;
    }
    fileSaveAsName = QDir::toNativeSeparators(fileSaveAsName);

    if (0 == lstLstContent.size())
    {
        QMessageBox::information(0,"info","No Data Need To Export");
        return false;
    }

    //load source xls
    QString sourceFileName = QDir::currentPath()+"./xlsSource/month_total.xlsx";
    sourceFileName = QDir::toNativeSeparators(QDir::cleanPath(sourceFileName));

    if (!QFile::exists(sourceFileName))
    {
        QMessageBox::critical(0,"Error",QString("The File Does Not Exist:%1").arg(sourceFileName));
        return false;
    }

    QExcel *pExcel = new QExcel(sourceFileName);
    if (!pExcel)
    {
        qDebug() << "Get Excel API Static Object Failed";
        return false;
    }

    //ȡ�ù���������
    if (pExcel->getSheetsCount() < 1)
    {
        QMessageBox::critical(0,"Error","Sheets Is Empty!");
        delete pExcel;
        return false;
    }
    //ȡ�õ�һ��������
    pExcel->selectSheet(SheetID);
    //ȡ�ù�������ʹ�÷�Χ
    int topLeftRow, topLeftColumn, bottomRightRow, bottomRightColumn;
    topLeftRow = 5;
    topLeftColumn = 1;
    bottomRightRow = lstLstContent.size();
    bottomRightColumn = lstLstContent.first().size();

    QProgressDialog progress("Exporting data...", "Abort Export", 0, bottomRightRow+1-topLeftRow, 0);
         progress.setWindowModality(Qt::WindowModal);

    for (int i=0;i<=bottomRightRow-topLeftRow;++i)
    {
        for (int j=0;j<=bottomRightColumn-topLeftColumn;++j)
        {
            pExcel->setCellVariant(i+topLeftRow, j+topLeftColumn, lstLstContent[i][j]);
        }
        progress.setValue(i-topLeftRow);
        if (progress.wasCanceled())
        {
            delete pExcel;
            return true;
        }
    }
    progress.setValue(bottomRightRow+1-topLeftRow);


    pExcel->saveAs(fileSaveAsName,"51");


    delete pExcel;

    QMessageBox::information(0,"Info",QString("Export Done: %1 ").arg(bottomRightRow+1-topLeftRow));

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
    //pExcel->getCellValue(2, 2).toString();
    //ɾ��������
    //pExcel->selectSheet("Sheet1");
    //pExcel->selectSheet(1);
    //pExcel->deleteSheet();
    //pExcel->save();
    //��������
    //pExcel->selectSheet("qExcelSheet3");
    //pExcel->setCellString(1, 7, "addString");
    //pExcel->setCellString("A3", "abc");
    //pExcel->save();
    //�ϲ���Ԫ��
    //pExcel->selectSheet(2);
    //pExcel->mergeCells("G1:H2");
    //pExcel->mergeCells(4, 7, 5 ,8);
    //pExcel->save();
    //�����п�
    //pExcel->selectSheet(1);
    //pExcel->setColumnWidth(1, 20);
    //pExcel->save();
    //���ô���
    //pExcel->selectSheet(1);
    //pExcel->setCellFontBold(2, 2, true);
    //pExcel->setCellFontBold("A2", true);
    //pExcel->save();
    //�������ִ�С
    //pExcel->selectSheet(1);
    //pExcel->setCellFontSize("B3", 20);
    //pExcel->setCellFontSize(1, 2, 20);
    //pExcel->save();
    //���õ�Ԫ�����־���
    //pExcel->selectSheet(2);
    //pExcel->setCellTextCenter(1, 2);
    //pExcel->setCellTextCenter("A2");
    //pExcel->save();
    //���õ�Ԫ�������Զ�����
    //pExcel->selectSheet(1);
    //pExcel->setCellTextWrap(2,2,true);
    //pExcel->setCellTextWrap("A2", true);
    //pExcel->save();
    //����һ������Ӧ�и�
    //pExcel->selectSheet(1);
    //pExcel->setAutoFitRow(2);
    //pExcel->save();
    //�½�������
    //pExcel->insertSheet("abc");
    //pExcel->save();
    //�����Ԫ������
    //pExcel->selectSheet(4);
    //pExcel->clearCell(1,1);
    //pExcel->clearCell("A2");
    //pExcel->save();
    //�ϲ�һ������ͬ�����ĵ�Ԫ��
    //pExcel->selectSheet(1);
    //pExcel->mergeSerialSameCellsInColumn(1, 2);
    //pExcel->save();
    //��ȡһ�Ź�������������
    //pExcel->selectSheet(1);
    //qDebug()<<pExcel->getUsedRowsCount();
    //�����и�
//    pExcel->selectSheet(1);
//    pExcel->setRowHeight(2, 30);
//    pExcel->save();
}
