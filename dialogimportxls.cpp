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
}

DialogImportXls::~DialogImportXls()
{
    delete ui;
}

bool DialogImportXls::getInfo(QString &inPutFile, int &titleEnd, int &sheetID, int &columnNum)
{
    if(ui->lineEdit_in->text().isEmpty())
    {
        return false;
    }
    inPutFile = ui->lineEdit_in->text();
    titleEnd = ui->spinBox_titleEnd->text().toInt();
    sheetID = ui->spinBox_sheetID->text().toInt();
    columnNum = ui->spinBox_column->text().toInt();

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
