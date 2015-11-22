#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include "qexcel.h"
#include "fpdbproc.h"
#include "fpexcelproc.h"

#include <QDebug>
#include <QString>
#include <QList>
#include <QMessageBox>


const QString FP_VERSION_NUM = "0.1.0.0";



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
    FpDbProc::releaseInstance();
    FpExcelProc::releaseInstance();

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
//    prepareDb();
//    setDataIntoDb();
//    getDataFromDb();
}

void MainWindow::onExportFile()
{
    //prepareExcel(false);
    setDataIntoExcel();
}


void MainWindow::procData()
{
    FpExcelProc *pFpExcelProc = FpExcelProc::getInstance();
    pFpExcelProc->procData();
}

void MainWindow::getDataFromExcel()
{
    FpExcelProc *pFpExcelProc = FpExcelProc::getInstance();
    pFpExcelProc->getDataFromExcel();
}

void MainWindow::setDataIntoExcel()
{
    FpExcelProc *pFpExcelProc = FpExcelProc::getInstance();
    pFpExcelProc->setDataIntoExcel();
}

bool MainWindow::prepareDb()
{
    bool retRes = true;
    FpDbProc *pFpDbProc = FpDbProc::getInstance();

    retRes  = retRes && pFpDbProc->prepareMemDb();
    retRes  = retRes && pFpDbProc->prepareLocalDb();

    return retRes;
}

bool MainWindow::getDataFromDb()
{
    bool retRes = true;
    FpDbProc *pFpDbProc = FpDbProc::getInstance();

    QList<QList<QVariant> > lstAllRes;
    retRes = retRes && pFpDbProc->getDataFromMemDb(lstAllRes);

    return retRes;
}

bool MainWindow::setDataIntoDb()
{
    bool retRes = true;
    FpDbProc *pFpDbProc = FpDbProc::getInstance();

    QList<QList<QVariant> > lstAllRes;
    retRes = retRes && pFpDbProc->setDataIntoMemDb(lstAllRes);

    return retRes;
}
