#include "fpdataproc.h"

#include "fpdbproc.h"
#include "fpexcelproc.h"

#include <QDateTime>
#include <QVariant>

const int OnDutyID = 7;
const int OffDutyID = 8;

FpDataProc::FpDataProc(QObject *parent) :
    QObject(parent)
{
    m_pFpDbProc = new FpDbProc;
    m_pFpExcelProc = new FpExcelProc;
}

FpDataProc::~FpDataProc()
{
    delete m_pFpDbProc;
    delete m_pFpExcelProc;
}

void FpDataProc::procData()
{
    QList<QVariant> lstTitle;
    QList<QList<QVariant> > lstLstContent;
    //need to do

    for(int i=0;i<lstLstContent.size();++i)
    {
        QDateTime dtTimeFlag;
        QDateTime dtStart = lstLstContent[i][OnDutyID].toDateTime();
        QDateTime dtEnd = lstLstContent[i][OffDutyID].toDateTime();
        if (dtStart.time() >= QTime(5,1))
        {
            dtTimeFlag = QDateTime(dtStart.date(),QTime(0,0));
        }
        else
        {
            dtTimeFlag = QDateTime(dtStart.date().addDays(-1),QTime(0,0));
        }

        //add timeflag
        lstLstContent[i] << QVariant(dtTimeFlag);

        //add total hour
        if (!dtEnd.isNull())
        {
            lstLstContent[i] << QVariant(dtStart.secsTo(dtEnd)*1.0/3600);
        }
        else
        {
            lstLstContent[i] << QVariant(0.0);
        }

        //day of week
        lstLstContent[i] << QVariant(dtTimeFlag.date().dayOfWeek());

        //abnormal work hours:0,normal;1,abnormal;2,beLate or leaveEarly
        if (dtEnd.isNull())
        {
            lstLstContent[i] << QVariant(1);
        }
        else
        {
            if(dtStart >= QDateTime(dtTimeFlag.date(),QTime(9,1))
                    || dtEnd < QDateTime(dtTimeFlag.date(),QTime(17,30)))
            {
                lstLstContent[i] << QVariant(2);
            }
            else
            {
                lstLstContent[i] << QVariant(0);//normal
            }
        }
        //qDebug() << lstLstContent[i];
    }
}

void FpDataProc::getDataFromExcel()
{
    ;
}

void FpDataProc::setDataIntoExcel()
{
    ;
}


void FpDataProc::prepareDb()
{
    m_pFpDbProc->prepareMemDb();
    m_pFpDbProc->prepareLocalDb();
}

void FpDataProc::getDataFromDb()
{
    m_pFpDbProc->getDataFromMemDb(m_lstLstContentDb);
}

void FpDataProc::setDataIntoDb()
{
    m_pFpDbProc->setDataIntoMemDb(m_lstLstContentDb);
}
