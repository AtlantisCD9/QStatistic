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
    syncWorkDays();
}

FpDataProc::~FpDataProc()
{
    delete m_pFpDbProc;
    delete m_pFpExcelProc;
}


void FpDataProc::procDataForDatail()
{
    QMap<QDate,int> mapDate2Type;
    foreach(QList<QVariant> workDay,m_lstRowLstColumnWorkDays)
    {
        mapDate2Type[workDay[0].toDate()] = workDay[1].toInt();
    }

    //QList<QVariant> lstTitle;
    for(int i=0;i<m_lstRowLstColumnDetail.size();++i)
    {
        QDateTime dtTimeFlag;
        QDateTime dtStart = m_lstRowLstColumnDetail[i][OnDutyID].toDateTime();
        QDateTime dtEnd = m_lstRowLstColumnDetail[i][OffDutyID].toDateTime();
        if (dtStart.time() >= QTime(5,1))
        {
            dtTimeFlag = QDateTime(dtStart.date(),QTime(0,0));
        }
        else
        {
            dtTimeFlag = QDateTime(dtStart.date().addDays(-1),QTime(0,0));
        }

        //timeflag
        m_lstRowLstColumnDetail[i] << QVariant(dtTimeFlag);

        //day_of_week
        m_lstRowLstColumnDetail[i] << QVariant(dtTimeFlag.date().dayOfWeek());

        //punch_hours
        if (!dtEnd.isNull())
        {
            m_lstRowLstColumnDetail[i] << QVariant(dtStart.secsTo(dtEnd)*1.0/3600);
        }
        else
        {
            m_lstRowLstColumnDetail[i] << QVariant(0.0);
        }

        //payroll_multi
        m_lstRowLstColumnDetail[i] << QVariant(mapDate2Type[dtTimeFlag.date()]);

        //charge_hours
        m_lstRowLstColumnDetail[i] << QVariant(0.0);


        //punch_type:0,normal;1,beLate or leaveEarly;2,abnormal;
        if (dtEnd.isNull())
        {
            m_lstRowLstColumnDetail[i] << QVariant(2);//abnormal
        }
        else
        {
            if(dtStart >= QDateTime(dtTimeFlag.date(),QTime(9,1))
                    || dtEnd < QDateTime(dtTimeFlag.date(),QTime(17,30)))
            {
               m_lstRowLstColumnDetail[i] << QVariant(1);//beLate or leaveEarly
            }
            else
            {
                m_lstRowLstColumnDetail[i] << QVariant(0);//normal
            }
        }



        //qDebug() << m_lstRowLstColumnDetail[i];
    }
}

void FpDataProc::procDataForCollection()
{
    ;
}

void FpDataProc::getDataFromExcel()
{
    m_pFpExcelProc->getDataFromExcel(m_lstTitleDetail,m_lstRowLstColumnDetail);
}

void FpDataProc::setDataIntoExcel()
{
    if (0 == m_lstRowLstColumnCollection.size())
    {
        return;
    }
    m_pFpExcelProc->setDataIntoExcel(m_lstTitleDetail,m_lstRowLstColumnCollection);
}


void FpDataProc::getWorkDays()
{
    if (!m_pFpDbProc->prepareLocalDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->getWorkDaysFromLocalDb(m_lstRowLstColumnWorkDays);
}

void FpDataProc::syncWorkDays()
{
    if (!m_pFpDbProc->prepareMemDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->setWorkDaysIntoMemDb(m_lstRowLstColumnWorkDays);

//    QList<QList<QVariant> > lstRowLstColumnWorkDays;
//    m_pFpDbProc->getWorkDaysFromMemDb(lstRowLstColumnWorkDays);
//    qDebug() << lstRowLstColumnWorkDays;
}

void FpDataProc::createWorkDays()
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

void FpDataProc::getDistinctPersonal()
{
    if (!m_pFpDbProc->prepareMemDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->getDutyDistinctPersonalFromMemDb(m_lstRowLstColumnCollection);
    //poid,job_id,name,SUM(punch_hours)
    m_lstTitleCollection << tr("��˾");
    m_lstTitleCollection << tr("����");
    m_lstTitleCollection << tr("��Ʒ��");
    m_lstTitleCollection << tr("POID");
    m_lstTitleCollection << tr("����");
    m_lstTitleCollection << tr("����");
    m_lstTitleCollection << tr("ʵ�ʴ򿨹�ʱ");

}

void FpDataProc::setDutyCollection()
{
    if (0 == m_lstRowLstColumnCollection.size())
    {
        return;
    }

    if (!m_pFpDbProc->prepareMemDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->setDutyCollectionIntoMemDb(m_lstRowLstColumnCollection);
}

void FpDataProc::setDutyDetail()
{
    if (0 == m_lstRowLstColumnDetail.size())
    {
        return;
    }

    if (!m_pFpDbProc->prepareMemDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->setDutyDetailIntoMemDb(m_lstRowLstColumnDetail);
}

const QString &FpDataProc::getDutyDetailSQL()
{
    return m_pFpDbProc->m_strDetailSQL;
}

const QString &FpDataProc::getDutyDetailBelateOrLeaveEarlySQL()
{
    return m_pFpDbProc->m_strDetailBelateOrLeaveEarlySQL;
}

const QString &FpDataProc::getDutyDetailMissPunchInSQL()
{
    return m_pFpDbProc->m_strDetailMissPunchInSQL;
}

const QString &FpDataProc::getDutyColletionSQL()
{
    return m_pFpDbProc->m_strCollectionSQL;
}
