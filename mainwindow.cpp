#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qexcel.h"
#include "dboper.h"

#include <QDebug>
#include <QDateTime>
#include <QString>
#include <QList>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>

const QString FP_VERSION_NUM="0.1.0.0";


const int SheetID = 1;
const int MaxRow = 100;
const int OnDutyID = 7;
const int OffDutyID = 8;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("FP Statistic");

    //set m_menuFile
    m_actionImportXls = new QAction(this);
    m_actionImportXls->setObjectName("m_actionImportXls");
    m_actionImportXls->setText(tr("&Import Xls"));

    m_actionExportXls = new QAction(this);
    m_actionExportXls->setObjectName("m_actionExportXls");
    m_actionExportXls->setText(tr("&Export Xls"));

    m_menuFile = new QMenu(ui->menuBar);
    m_menuFile->setObjectName("m_menuFile");
    m_menuFile->setTitle(tr("&File"));

    m_menuFile->addAction(m_actionImportXls);
    m_menuFile->addAction(m_actionExportXls);

    //set m_menuConfig
    m_actionWorkDaysSetting = new QAction(this);
    m_actionWorkDaysSetting->setObjectName("m_actionWorkDaysSetting");
    m_actionWorkDaysSetting->setText(tr("&Work Days Setting"));

    m_menuConfig = new QMenu(ui->menuBar);
    m_menuConfig->setObjectName("m_menuConfig");
    m_menuConfig->setTitle(tr("&Config"));

    m_menuConfig->addAction(m_actionWorkDaysSetting);

    //set m_menuHelp
    m_actionAbout = new QAction(this);
    m_actionAbout->setObjectName("m_actionAbout");
    m_actionAbout->setText(tr("&About"));

    m_menuHelp = new QMenu(ui->menuBar);
    m_menuHelp->setObjectName("m_menuHelp");
    m_menuHelp->setTitle(tr("&Help"));

    m_menuHelp->addAction(m_actionAbout);


    //add menus on menuBar
    ui->menuBar->addAction(m_menuFile->menuAction());
    ui->menuBar->addAction(m_menuConfig->menuAction());
    ui->menuBar->addAction(m_menuHelp->menuAction());

    //add actions on ToolBar
    if (0 == ui->mainToolBar->actions().size())
    {
        ui->mainToolBar->setHidden(true);
    }


    //set connection
    connect(m_actionImportXls,SIGNAL(triggered()),
            this,SLOT(onImportFile()));

    connect(m_actionExportXls,SIGNAL(triggered()),
            this,SLOT(onExportFile()));

    connect(m_actionAbout,SIGNAL(triggered()),
            this,SLOT(onAbout()));

}

MainWindow::~MainWindow()
{
    QExcel::releaseInstance();
    DbOper::releaseInstance();

    delete ui;
}

void MainWindow::onAbout()
{
    QMessageBox::about(this,
                       tr("About Application"),
                       QString(tr("FP Statistic Tool\nVersion %1").arg(FP_VERSION_NUM)));
}

void MainWindow::onImportFile()
{
    getDataFromExcel();
    procData();
    prepareDb();
    setDataIntoDb();
    getDataFromDb();
}

void MainWindow::onExportFile()
{
    //prepareExcel(false);
    setDataIntoExcel();
}

bool MainWindow::getDataFromExcel()
{
    QString fileName = QFileDialog::getOpenFileName(this,
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
        QMessageBox::critical(this,"Error",QString("The File Does Not Exist:%1").arg(fileName));
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
        QMessageBox::critical(this,"Error","Sheets Is Empty!");
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


    QProgressDialog progress("Loading data...", "Abort Load", 0, bottomRightRow+1-topLeftRow, this);
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

bool MainWindow::setDataIntoExcel()
{
    QString fileName = QFileDialog::getSaveFileName(this,
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
        QMessageBox::information(this,"info","没有需要到处的数据");
        return false;
    }

    QString sourceFileName;
    sourceFileName = QDir::currentPath()+"./xlsSource/month_total.xlsx";
    sourceFileName = QDir::toNativeSeparators(QDir::cleanPath(fileName));

    if (!QFile::exists(fileName))
    {
        QMessageBox::critical(this,"Error",QString("The File Does Not Exist:%1").arg(fileName));
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
        QMessageBox::critical(this,"Error","Sheets Is Empty!");
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

    QProgressDialog progress("Exporting data...", "Abort Export", 0, bottomRightRow+1-topLeftRow, this);
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


bool MainWindow::prepareDb()
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = DbOper::getInstance();
    retRes = retRes && dbOper->dbOpen();
    if (!retRes)
    {
        return retRes;
    }

    strSql = "Drop Table duty_detail";
    dbOper->dbQureyExec(strSql);

    strSql =
            "Create Table duty_detail (                "
            "    company             vchar,            "
            "    area                vchar,            "
            "    product_line        vchar,            "
            "    sub_product_line    vchar,            "
            "    PDU_SPDT            vchar,            "
            "    job_id              vchar,            "
            "    name                vchar,            "
            "    on_duty             datetime,         "
            "    off_duty            datetime,         "
            "    collaboration_type  vchar,            "
            "    ID_number           vchar Not Null,   "
            "    POID                vchar,            "
            "    timeflag            datetime,         "
            "    work_hours          double,           "
            "    day_of_week         int,              "
            "    work_hours_type     int,              "
            "    PRIMARY KEY(ID_number,timeflag)       "
            ")                                        ";
    retRes  = retRes && dbOper->dbQureyExec(strSql);


    return retRes;
}


bool MainWindow::getDataFromDb()
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = DbOper::getInstance();

    QList<QList<QVariant> > lstAllRes;
    strSql = "select distinct poid,job_id,name,sum(work_hours) "
            "from duty_detail where work_hours_type = 0 group by ID_number,poid order by poid,job_id";
    retRes = retRes && dbOper->dbQureyData(strSql,lstAllRes);

//    foreach(QList<QVariant> lstRes,lstAllRes)
//    {
//        qDebug() << lstRes;
//    }



    return retRes;

}

bool MainWindow::setDataIntoDb()
{
    bool retRes = true;
    QString strSql;
    DbOper *dbOper = DbOper::getInstance();

    strSql = "Insert Into duty_detail Values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
    retRes = retRes && dbOper->dbInsertData(strSql,m_lstStrLstContent);

    return retRes;



//    QList<QList<QVariant> > lstResult;
//    strSql = QString("select * from duty_detail");
//    dbOper->dbQurey(strSql,lstResult);
//    qDebug() << lstResult;



//    DbOper::releaseInstance();


}




void MainWindow::procData()
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
