#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "fpdataproc.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlTableModel>


const QString FP_VERSION_NUM = "0.1.0.0";



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
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
    delete m_pFpDataProc;
    delete ui;
}

void MainWindow::initialMenuFile()
{
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

void MainWindow::onImportFile()
{
    m_pFpDataProc->getDataFromExcel();
    m_pFpDataProc->procData();
    m_pFpDataProc->setDutyDetail();

    QSqlDatabase db = QSqlDatabase::database(":memory:");
    QSqlTableModel *model = new QSqlTableModel(this, db);
     model->setTable("duty_detail");
     //model->setEditStrategy(QSqlTableModel::OnManualSubmit);
     model->select();
//         model->setHeaderData(0, Qt::Horizontal, tr("Name"));
//         model->setHeaderData(1, Qt::Horizontal, tr("Salary"));

     ui->tableView->setModel(model);
//         view->setModel(model);
//         view->hideColumn(0); // don't show the ID
     ui->tableView->show();
}

void MainWindow::onExportFile()
{
    m_pFpDataProc->getDutyCollection();
    m_pFpDataProc->setDataIntoExcel();
}


