#include "dialogstatistics.h"
#include "ui_dialogstatistics.h"

#include <QMessageBox>

DialogStatistics::DialogStatistics(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogStatistics)
{
    ui->setupUi(this);

    QDate curDate = QDate::currentDate();
    QDate startDate = QDate(curDate.year(),curDate.month(),1);
    QDate endDate = QDate(curDate.addMonths(1).year(),curDate.addMonths(1).month(),1).addDays(-1);

    setDate(startDate,endDate);
}

DialogStatistics::~DialogStatistics()
{
    delete ui;
}

void DialogStatistics::setInfo(int detail, int abnormal, int poSwitch)
{
    if(detail > 0)
    {
        ui->checkBox_detail->setChecked(true);
    }
    else
    {
        ui->checkBox_detail->setChecked(false);
    }

    if(abnormal > 0)
    {
        ui->checkBox_abnormal->setChecked(true);
    }
    else
    {
        ui->checkBox_abnormal->setChecked(false);
    }

    if(poSwitch > 0)
    {
        ui->checkBox_poSwitch->setChecked(true);
    }
    else
    {
        ui->checkBox_poSwitch->setChecked(false);
    }
}

void DialogStatistics::setDate(QDate &startDate, QDate &endDate)
{
    ui->dateEdit_start->setDate(startDate);
    ui->dateEdit_end->setDate(endDate);
}

bool DialogStatistics::getInfo(QDate &startDate, QDate &endDate)
{
    startDate = ui->dateEdit_start->date();
    endDate = ui->dateEdit_end->date();

    if(startDate.month() != endDate.month())
    {
        QMessageBox::information(this,"提示","不可跨月度统计");
        return false;
    }

    return true;
}
