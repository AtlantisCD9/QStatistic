#include "dialogmergexls.h"
#include "ui_dialogmergexls.h"

#include <QFileDialog>

DialogMergeXls::DialogMergeXls(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMergeXls)
{
    ui->setupUi(this);

    connect(ui->pushButton_in,SIGNAL(clicked()),
            this,SLOT(onGetOpenFileNames()));
    connect(ui->pushButton_out,SIGNAL(clicked()),
            this,SLOT(onGetSaveFileName()));

}

DialogMergeXls::~DialogMergeXls()
{
    delete ui;
}

bool DialogMergeXls::getInfo(QStringList &lstMergeFile, QString &outPutFile, int &titleEnd, int &sheetID, int &columnNum)
{
    if(ui->lineEdit_in->text().isEmpty() || ui->lineEdit_out->text().isEmpty())
    {
        return false;
    }
    lstMergeFile = ui->lineEdit_in->text().split(";");
    outPutFile = ui->lineEdit_out->text();
    titleEnd = ui->spinBox_titleEnd->text().toInt();
    sheetID = ui->spinBox_sheetID->text().toInt();
    columnNum = ui->spinBox_column->text().toInt();

    return true;
}


void DialogMergeXls::onGetOpenFileNames()
{
    QStringList lstFileName;
    lstFileName = QFileDialog::getOpenFileNames(this,
                                                tr("Merge Excel"),
                                                QString(),
                                                tr("Excel Files (*.xls *.xlsx)"));

    if (lstFileName.isEmpty())
    {
        //user press cancel
        ui->lineEdit_in->setText(QString());
        return;
    }
    ui->lineEdit_in->setText(lstFileName.join(";"));
}


void DialogMergeXls::onGetSaveFileName()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,
                                          tr("Export Excel"),
                                          QString(),
                                          tr("Excel Files (*.xlsx *.xls)"));

    if (fileName.isNull())
    {
        //user press cancel;
        ui->lineEdit_out->setText(QString());
        return;
    }
    fileName = QDir::toNativeSeparators(fileName);
    ui->lineEdit_out->setText(fileName);
}

