#include "fpdataproc.h"

#include "fpdbproc.h"
#include "fpexcelproc.h"

#include <QMap>
#include <QDate>
#include <QDateTime>
#include <QVariant>
#include <QDebug>

const int OnDutyID = 7;
const int OffDutyID = 8;

FpDataProc::FpDataProc(QObject *parent) :
    QObject(parent)
{
    m_pFpDbProc = new FpDbProc;
    m_pFpExcelProc = new FpExcelProc;

    //setWorkDays();
    getWorkDays();
}

FpDataProc::~FpDataProc()
{
    delete m_pFpDbProc;
    delete m_pFpExcelProc;
}

void FpDataProc::procData()
{
    QMap<QDate,int> mapDate2Type;
    foreach(QList<QVariant> workDay,m_lstLstContentWorkDays)
    {
        mapDate2Type[workDay[0].toDate()] = workDay[1].toInt();
    }

    //QList<QVariant> lstTitle;
    for(int i=0;i<m_lstLstContentExcel.size();++i)
    {
        QDateTime dtTimeFlag;
        QDateTime dtStart = m_lstLstContentExcel[i][OnDutyID].toDateTime();
        QDateTime dtEnd = m_lstLstContentExcel[i][OffDutyID].toDateTime();
        if (dtStart.time() >= QTime(5,1))
        {
            dtTimeFlag = QDateTime(dtStart.date(),QTime(0,0));
        }
        else
        {
            dtTimeFlag = QDateTime(dtStart.date().addDays(-1),QTime(0,0));
        }

        //add timeflag
        m_lstLstContentExcel[i] << QVariant(dtTimeFlag);

        //add total hour
        if (!dtEnd.isNull())
        {
            m_lstLstContentExcel[i] << QVariant(dtStart.secsTo(dtEnd)*1.0/3600);
        }
        else
        {
            m_lstLstContentExcel[i] << QVariant(0.0);
        }

        //day of week
        m_lstLstContentExcel[i] << QVariant(dtTimeFlag.date().dayOfWeek());

        //abnormal work hours:0,normal;1,abnormal;2,beLate or leaveEarly
        if (dtEnd.isNull())
        {
            m_lstLstContentExcel[i] << QVariant(1);
        }
        else
        {
            if(dtStart >= QDateTime(dtTimeFlag.date(),QTime(9,1))
                    || dtEnd < QDateTime(dtTimeFlag.date(),QTime(17,30)))
            {
                m_lstLstContentExcel[i] << QVariant(2);
            }
            else
            {
                m_lstLstContentExcel[i] << QVariant(0);//normal
            }
        }
        //qDebug() << m_lstLstContentExcel[i];
    }
}

void FpDataProc::getDataFromExcel()
{
    m_pFpExcelProc->getDataFromExcel(m_lstTitleExcel,m_lstLstContentExcel);
}

void FpDataProc::setDataIntoExcel()
{
    if (0 == m_lstLstContentDutyCollection.size())
    {
        return;
    }
    m_pFpExcelProc->setDataIntoExcel(m_lstTitleExcel,m_lstLstContentDutyCollection);
}


void FpDataProc::getWorkDays()
{
    if (!m_pFpDbProc->prepareLocalDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->getWorkDaysFromLocalDb(m_lstLstContentWorkDays);
}

void FpDataProc::setWorkDays()
{
    QList<QList<QVariant> > lstLstContentWorkDays;
    QMap<int,int> mapWeek2Type;
    mapWeek2Type[1] = 1;
    mapWeek2Type[2] = 1;
    mapWeek2Type[3] = 1;
    mapWeek2Type[4] = 1;
    mapWeek2Type[5] = 1;
    mapWeek2Type[6] = 2;
    mapWeek2Type[7] = 2;

    int yearNum = 2015;
    QDate endDate(yearNum+1,1,1);
    QDate myDate(yearNum,1,1);
    while(myDate < endDate)
    {
        lstLstContentWorkDays << QList<QVariant>();
        lstLstContentWorkDays.last() << myDate;
        lstLstContentWorkDays.last() << mapWeek2Type[myDate.dayOfWeek()];

        myDate = myDate.addDays(1);
    }

    if (!m_pFpDbProc->prepareLocalDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->setWorkDaysIntoLocalDb(lstLstContentWorkDays);
}

void FpDataProc::getDutyCollection()
{
    if (!m_pFpDbProc->prepareMemDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->getDutyCollectionFromMemDb(m_lstLstContentDutyCollection);
}

void FpDataProc::setDutyDetail()
{
    if (0 == m_lstLstContentExcel.size())
    {
        return;
    }

    if (!m_pFpDbProc->prepareMemDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->setDutyDetailIntoMemDb(m_lstLstContentExcel);
}
