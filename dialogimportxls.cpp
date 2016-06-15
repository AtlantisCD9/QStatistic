#include "dialogimportxls.h"
#include "ui_dialogimportxls.h"

#include <QFileDialog>

DialogImportXls::DialogImportXls(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogImportXls)
{
    ui->setupUi(this);

    connect(ui->pushButton_in_detail,SIGNAL(clicked()),
            this,SLOT(onGetOpenFileName()));
    connect(ui->pushButton_in_abnormal,SIGNAL(clicked()),
            this,SLOT(onGetOpenFileName()));
    connect(ui->pushButton_in_poSwitch,SIGNAL(clicked()),
            this,SLOT(onGetOpenFileName()));

    onRefresh();
}

DialogImportXls::~DialogImportXls()
{
    delete ui;
}

bool DialogImportXls::getInfo(QString &inPutFile, int &titleEnd, int &sheetID, int &columnNum, ENUM_IMPORT_XLS_TYPE &type)
{
    switch(type)
    {
    case IM_DETAIL:
        if(ui->lineEdit_in_detail->text().isEmpty())
        {
            return false;
        }
        inPutFile = ui->lineEdit_in_detail->text();
        titleEnd = ui->spinBox_titleEnd_detail->text().toInt();
        sheetID = ui->spinBox_sheetID_detail->text().toInt();
        columnNum = ui->spinBox_column_detail->text().toInt();
        break;
    case IM_ABNORMAL:
        if(ui->lineEdit_in_abnormal->text().isEmpty())
        {
            return false;
        }
        inPutFile = ui->lineEdit_in_abnormal->text();
        titleEnd = ui->spinBox_titleEnd_abnormal->text().toInt();
        sheetID = ui->spinBox_sheetID_abnormal->text().toInt();
        columnNum = ui->spinBox_column_abnormal->text().toInt();
        break;
    case IM_PO_SWITCH:
        if(ui->lineEdit_in_poSwitch->text().isEmpty())
        {
            return false;
        }
        inPutFile = ui->lineEdit_in_poSwitch->text();
        titleEnd = ui->spinBox_titleEnd_poSwitch->text().toInt();
        sheetID = ui->spinBox_sheetID_poSwitch->text().toInt();
        columnNum = ui->spinBox_column_poSwitch->text().toInt();
        break;
    default:
        return false;
    }

    return true;
}

void DialogImportXls::onGetOpenFileName()
{
    QLineEdit *curLineEdit;//ui->lineEdit_in_detail

    QString senderName = sender()->objectName();

    if (senderName.contains("detail"))
    {
        curLineEdit = ui->lineEdit_in_detail;
    }
    else if (senderName.contains("abnormal"))
    {
        curLineEdit = ui->lineEdit_in_abnormal;
    }
    else if (senderName.contains("poSwitch"))
    {
        curLineEdit = ui->lineEdit_in_poSwitch;
    }
    else
    {
        return;
    }

    QString fileName;
    fileName = QFileDialog::getOpenFileName(0,
                                            tr("Import Excel"),
                                            QString(),
                                            tr("Excel Files (*.xls *.xlsx)"));

    if (fileName.isNull())
    {
        //user press cancel
        curLineEdit->setText(QString());
        return;
    }
    curLineEdit->setText(fileName);
}

void DialogImportXls::onRefresh()
{
    ui->spinBox_titleEnd_detail->setValue(1);
    ui->spinBox_column_detail->setValue(12);

    ui->spinBox_titleEnd_abnormal->setValue(4);
    ui->spinBox_column_abnormal->setValue(6);

    ui->spinBox_titleEnd_poSwitch->setValue(5);
    ui->spinBox_column_poSwitch->setValue(5);
}
