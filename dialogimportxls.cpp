#include "dialogimportxls.h"
#include "ui_dialogimportxls.h"

#include <QFileDialog>

DialogImportXls::DialogImportXls(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogImportXls)
{
    ui->setupUi(this);

    connect(ui->pushButton_in,SIGNAL(clicked()),
            this,SLOT(onGetOpenFileName()));

    connect(ui->radioButton_detail,SIGNAL(clicked()),
            this,SLOT(onRefresh()));
    connect(ui->radioButton_abnormal,SIGNAL(clicked()),
            this,SLOT(onRefresh()));
    connect(ui->radioButton_poSwitch,SIGNAL(clicked()),
            this,SLOT(onRefresh()));

    onRefresh();
}

DialogImportXls::~DialogImportXls()
{
    delete ui;
}

bool DialogImportXls::getInfo(QString &inPutFile, int &titleEnd, int &sheetID, int &columnNum, ENUM_IMPORT_XLS_TYPE &type)
{
    if(ui->lineEdit_in->text().isEmpty())
    {
        return false;
    }
    inPutFile = ui->lineEdit_in->text();
    titleEnd = ui->spinBox_titleEnd->text().toInt();
    sheetID = ui->spinBox_sheetID->text().toInt();
    columnNum = ui->spinBox_column->text().toInt();

    if(ui->radioButton_detail->isChecked())
    {
        type = IM_DETAIL;
    }
    else if(ui->radioButton_abnormal->isChecked())
    {
        type = IM_ABNORMAL;
    }
    else
    {
        type = IM_PO_SWITCH;
    }

    return true;
}

void DialogImportXls::onGetOpenFileName()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(0,
                                            tr("Import Excel"),
                                            QString(),
                                            tr("Excel Files (*.xls *.xlsx)"));

    if (fileName.isNull())
    {
        //user press cancel
        ui->lineEdit_in->setText(QString());
        return;
    }
    ui->lineEdit_in->setText(fileName);
}

void DialogImportXls::onRefresh()
{
    if(ui->radioButton_detail->isChecked())
    {
        ui->spinBox_titleEnd->setValue(1);
        ui->spinBox_column->setValue(12);
    }
    else if(ui->radioButton_abnormal->isChecked())
    {
        ui->spinBox_titleEnd->setValue(4);
        ui->spinBox_column->setValue(6);
    }
    else
    {
        ui->spinBox_titleEnd->setValue(5);
        ui->spinBox_column->setValue(5);
    }
}
