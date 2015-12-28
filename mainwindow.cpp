#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogimportxls.h"
#include "dialogmergexls.h"
#include "dialogexportxls.h"

#include "fpdataproc.h"
#include "globaldef.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QStandardItemModel>
#include <QStandardItem>


const QString FP_VERSION_NUM = "0.1.1.0";



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_importXls(new DialogImportXls),
    m_mergeXls(new DialogMergeXls),
    m_exportXls(new DialogExportXls),
    m_pFpDataProc(new FpDataProc)
{
    ui->setupUi(this);

    this->setWindowTitle("FP Statistic");

    initialMenuFile();
    initialMenuConfig();
    initialMenuHelp();

    //add menus on menuBar
    ui->menuBar->addAction(m_menuFile->menuAction());
    ui->menuBar->addAction(m_menuConfig->menuAction());
    ui->menuBar->addAction(m_menuHelp->menuAction());

    //add actions on ToolBar
    if (0 == ui->mainToolBar->actions().size())
    {
        ui->mainToolBar->setHidden(true);
    }

    m_actionWorkDaysSetting->setDisabled(true);
    m_actionExportXls->setDisabled(true);


    //set connection
    connect(m_actionImportXls,SIGNAL(triggered()),
            this,SLOT(onImportFile()));

    connect(m_actionMergeXls,SIGNAL(triggered()),
            this,SLOT(onMergeFile()));

    connect(m_actionExportXls,SIGNAL(triggered()),
            this,SLOT(onExportFile()));

    connect(m_actionAbout,SIGNAL(triggered()),
            this,SLOT(onAbout()));

}

MainWindow::~MainWindow()
{
    delete m_pFpDataProc;
    delete m_exportXls;
    delete m_mergeXls;
    delete m_importXls;
    delete ui;
}

void MainWindow::initialMenuFile()
{
    //set m_menuFile
    m_actionImportXls = new QAction(this);
    m_actionImportXls->setObjectName("m_actionImportXls");
    m_actionImportXls->setText(tr("&Import Xls"));

    m_actionMergeXls = new QAction(this);
    m_actionMergeXls->setObjectName("m_actionMergeXls");
    m_actionMergeXls->setText(tr("&Merge Xls"));

    m_actionExportXls = new QAction(this);
    m_actionExportXls->setObjectName("m_actionExportXls");
    m_actionExportXls->setText(tr("&Export Xls"));

    m_menuFile = new QMenu(ui->menuBar);
    m_menuFile->setObjectName("m_menuFile");
    m_menuFile->setTitle(tr("&File"));

    m_menuFile->addAction(m_actionImportXls);
    m_menuFile->addAction(m_actionMergeXls);
    m_menuFile->addAction(m_actionExportXls);
}

void MainWindow::initialMenuConfig()
{
    //set m_menuConfig
    m_actionWorkDaysSetting = new QAction(this);
    m_actionWorkDaysSetting->setObjectName("m_actionWorkDaysSetting");
    m_actionWorkDaysSetting->setText(tr("&Work Days Setting"));

    m_menuConfig = new QMenu(ui->menuBar);
    m_menuConfig->setObjectName("m_menuConfig");
    m_menuConfig->setTitle(tr("&Config"));

    m_menuConfig->addAction(m_actionWorkDaysSetting);
}

void MainWindow::initialMenuHelp()
{
    //set m_menuHelp
    m_actionAbout = new QAction(this);
    m_actionAbout->setObjectName("m_actionAbout");
    m_actionAbout->setText(tr("&About"));

    m_menuHelp = new QMenu(ui->menuBar);
    m_menuHelp->setObjectName("m_menuHelp");
    m_menuHelp->setTitle(tr("&Help"));

    m_menuHelp->addAction(m_actionAbout);
}

void MainWindow::onAbout()
{
    QMessageBox::about(this,
                       tr("About Application"),
                       QString(tr("FP Statistic Tool\nVersion %1").arg(FP_VERSION_NUM)));
}

void MainWindow::showDetail(QTableView *tableWidget)
{
    if (NULL == tableWidget)
    {
        return;
    }

    QSqlDatabase db = QSqlDatabase::database(DB_NAME);
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(m_pFpDataProc->getDutyDetailSQL(),db);

    if(model->lastError().isValid())
    {
        qDebug() << model->lastError();
    }

    int iSize = m_pFpDataProc->getDutyDetailTitle().size();
    for(int i = 0;i < iSize;++i)
    {
        model->setHeaderData(i, Qt::Horizontal, m_pFpDataProc->getDutyDetailTitle()[i].toString());
    }
    tableWidget->setModel(model);
    tableWidget->setColumnWidth(0,50);
    tableWidget->setColumnWidth(1,50);
    tableWidget->setColumnWidth(6,60);
    tableWidget->setColumnWidth(7,150);
    tableWidget->setColumnWidth(8,150);
    tableWidget->setColumnWidth(10,150);
    tableWidget->show();
}

void MainWindow::showDetailBelateOrLeaveEarly(QTableView *tableWidget)
{
    if (NULL == tableWidget)
    {
        return;
    }

    QSqlDatabase db = QSqlDatabase::database(DB_NAME);
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(m_pFpDataProc->getDutyDetailBelateOrLeaveEarlySQL(),db);

    if(model->lastError().isValid())
    {
        qDebug() << model->lastError();
    }

    int iSize = m_pFpDataProc->getDutyDetailTitle().size();
    for(int i = 0;i < iSize;++i)
    {
        model->setHeaderData(i, Qt::Horizontal, m_pFpDataProc->getDutyDetailTitle()[i].toString());
    }

    tableWidget->setModel(model);
    tableWidget->setColumnWidth(0,50);
    tableWidget->setColumnWidth(1,50);
    tableWidget->setColumnWidth(6,60);
    tableWidget->setColumnWidth(7,150);
    tableWidget->setColumnWidth(8,150);
    tableWidget->setColumnWidth(10,150);
    tableWidget->show();
}

void MainWindow::showDetailMissPunchIn(QTableView *tableWidget)
{
    if (NULL == tableWidget)
    {
        return;
    }

    QSqlDatabase db = QSqlDatabase::database(DB_NAME);
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(m_pFpDataProc->getDutyDetailMissPunchInSQL(),db);

    if(model->lastError().isValid())
    {
        qDebug() << model->lastError();
    }

    int iSize = m_pFpDataProc->getDutyDetailTitle().size();
    for(int i = 0;i < iSize;++i)
    {
        model->setHeaderData(i, Qt::Horizontal, m_pFpDataProc->getDutyDetailTitle()[i].toString());
    }

    tableWidget->setModel(model);
    tableWidget->setColumnWidth(0,50);
    tableWidget->setColumnWidth(1,50);
    tableWidget->setColumnWidth(6,60);
    tableWidget->setColumnWidth(7,150);
    tableWidget->setColumnWidth(8,150);
    tableWidget->setColumnWidth(10,150);
    tableWidget->show();
}

void MainWindow::showCollection(QTableView *tableWidget)
{
    if (NULL == tableWidget)
    {
        return;
    }

    QStandardItemModel *model = new QStandardItemModel(this);

    QList<QList<QVariant> > lstRowLstColumnCollection =  m_pFpDataProc->getDutyColletionLstRowLstColumn();
    foreach(QList<QVariant> lstColumn,lstRowLstColumnCollection)
    {
        QList<QStandardItem *> lstItem;
        foreach(QVariant column,lstColumn)
        {
            QStandardItem *pItem = new QStandardItem();
            pItem->setData(column,Qt::DisplayRole);
            pItem->setEditable(false);
            lstItem << pItem;
        }
        model->appendRow(lstItem);
    }


    int iSize = m_pFpDataProc->getDutyColletionTitle().size();
    for(int i = 0;i < iSize;++i)
    {
        model->setHeaderData(i, Qt::Horizontal, m_pFpDataProc->getDutyColletionTitle()[i]);
    }

    tableWidget->setModel(model);

    tableWidget->setColumnWidth(4,150);
    tableWidget->show();
}

void MainWindow::onImportFile()
{
    if(!m_importXls->exec())
    {
        return;
    }

    QString inPutFile;
    int headEnd;
    int sheetID;
    int columnNum;

//    qDebug() << m_mergeXls->getInfo(lstMergeFile,outPutFile,headEnd);
//    qDebug() << lstMergeFile;
//    qDebug() << outputFile;
//    qDebug() << headEnd;
    if(!m_importXls->getInfo(inPutFile,headEnd,sheetID,columnNum))
    {
        return;
    }

    m_pFpDataProc->initial();
    m_pFpDataProc->getDataFromExcel(inPutFile,headEnd,sheetID,columnNum);

    m_pFpDataProc->procDataForDatail();
    m_pFpDataProc->setDutyDetail();

    m_pFpDataProc->getBelateOrLeaveEarlyDetail();
    m_pFpDataProc->getMissPunchInDetail();

    m_pFpDataProc->getDistinctPersonal();
    m_pFpDataProc->procDataForCollection();

    showDetail(ui->tableView_detail);
    showDetailBelateOrLeaveEarly(ui->tableView_beLateOrLeaveEarly);
    showDetailMissPunchIn(ui->tableView_missPunchIn);
    showCollection(ui->tableView_monthCollect);

    m_actionExportXls->setEnabled(true);
    m_actionImportXls->setDisabled(true);

}

void MainWindow::onMergeFile()
{
    if(!m_mergeXls->exec())
    {
        return;
    }

    QStringList lstMergeFile;
    QString outPutFile;
    int headEnd;
    int sheetID;
    int columnNum;

    if(!m_mergeXls->getInfo(lstMergeFile,outPutFile,headEnd,sheetID,columnNum))
    {
        return;
    }
//    qDebug() << lstMergeFile;
//    qDebug() << outPutFile;
//    qDebug() << headEnd;
//    qDebug() << sheetID;
//    qDebug() << columnNum;

    m_pFpDataProc->mergeExcel(lstMergeFile,outPutFile,headEnd,sheetID,columnNum);
}

void MainWindow::onExportFile()
{
    if(!m_exportXls->exec())
    {
        return;
    }

    QString outPutFile;
    int sheetID;

//    qDebug() << m_mergeXls->getInfo(lstMergeFile,outPutFile,headEnd);
//    qDebug() << lstMergeFile;
//    qDebug() << outputFile;
//    qDebug() << headEnd;
    if(!m_exportXls->getInfo(outPutFile,sheetID))
    {
        return;
    }

    m_pFpDataProc->setDataIntoExcel(outPutFile,sheetID);

}


