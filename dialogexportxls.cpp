#include "dialogexportxls.h"
#include "ui_dialogexportxls.h"

#include <QFileDialog>

DialogExportXls::DialogExportXls(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExportXls)
{
    ui->setupUi(this);

    connect(ui->pushButton_out,SIGNAL(clicked()),
            this,SLOT(onGetSaveFileName()));
}

DialogExportXls::~DialogExportXls()
{
    delete ui;
}

bool DialogExportXls::getInfo(QString &outPutFile, int &sheetID)
{
    if(ui->lineEdit_out->text().isEmpty())
    {
        return false;
    }
    outPutFile = ui->lineEdit_out->text();
    sheetID = ui->spinBox_sheetID->text().toInt();

    return true;
}

void DialogExportXls::onGetSaveFileName()
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
