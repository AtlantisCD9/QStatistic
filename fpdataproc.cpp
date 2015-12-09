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

void FpDataProc::initial()
{
    m_lstTitleDetail.clear();//明细抬头
    m_lstTitleCollection.clear();//汇总抬头

    m_lstRowLstColumnDetail.clear();//will be add some proc data

    m_lstRowLstColumnBelateOrLeaveEarlyDetail.clear();//will be add some proc data
    m_lstRowLstColumnMissPunchInDetail.clear();//will be add some proc data
    m_lstRowLstColumnCollection.clear();//for month collection will be add some proc data
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

        //abnormal_hours
        if (m_lstRowLstColumnDetail[i].last() == QVariant(2))
        {
            if (1 == mapDate2Type[dtTimeFlag.date()])
            {
                m_lstRowLstColumnDetail[i] << QVariant(8);
            }
            else
            {
                m_lstRowLstColumnDetail[i] << QVariant(0);
            }

        }
        else if (m_lstRowLstColumnDetail[i].last() == QVariant(1))
        {
            if (1 == mapDate2Type[dtTimeFlag.date()])
            {
                int secsTemp = 8*3600-getAbnormalHours(dtTimeFlag,dtStart,dtEnd);
                if (secsTemp < 0)
                {
                    secsTemp = 0;
                }

                m_lstRowLstColumnDetail[i] << QVariant(secsTemp*1.0/3600);
            }
            else
            {
                m_lstRowLstColumnDetail[i] << QVariant(0);
            }
        }
        else
        {
            m_lstRowLstColumnDetail[i] << QVariant(0);
        }

        //punch_hours
        if (!dtEnd.isNull())
        {
            if (1 == mapDate2Type[dtTimeFlag.date()])
            {
                m_lstRowLstColumnDetail[i] << QVariant(getPunchInHours(dtTimeFlag,dtStart,dtEnd)*1.0/3600);
            }
            else
            {
                int overTimeHours = 8*3600;
                if (getOverTimeHours(dtTimeFlag,dtStart,dtEnd) < overTimeHours)
                {
                    overTimeHours = getOverTimeHours(dtTimeFlag,dtStart,dtEnd);
                }
                m_lstRowLstColumnDetail[i] << QVariant(overTimeHours*1.0/3600);
            }
        }
        else
        {
            m_lstRowLstColumnDetail[i] << QVariant(0.0);
        }

        //payroll_multi
        m_lstRowLstColumnDetail[i] << QVariant(mapDate2Type[dtTimeFlag.date()]);

        //charge_hours
        m_lstRowLstColumnDetail[i] << QVariant(0.0);






        //qDebug() << m_lstRowLstColumnDetail[i];
    }
}

void FpDataProc::procDataForCollection()
{
    //检查是否处于某个月
    if (!getAndCheckCurMonth())
    {
        return;
    }

    //如果是某个月，那么获取该月份的日历表
    QList<QList<QVariant> > lstStrLstContent;
    m_pFpDbProc->getWorkDaysByCurMonthFromMemDb(lstStrLstContent,m_strDateMonth);

    QMap<QDate,int> mapDate2Type;
    foreach(QList<QVariant> workDay,lstStrLstContent)
    {
        mapDate2Type[workDay[0].toDate()] = workDay[1].toInt();
    }

    int workDaysNum = 0;
    foreach(QList<QVariant> workDay,lstStrLstContent)
    {
        if (1 == workDay[1].toInt())
        {
            ++workDaysNum;
        }
    }

    m_lstTitleCollection << QVariant(QString("应服务工时"));
    m_lstTitleCollection << QVariant(QString("欠工时"));


    //初始化dayOfWeek到文字描述映射
    QMap<int,QString> mapInt2Descrp;
    mapInt2Descrp[1] = tr("周一");//Monday
    mapInt2Descrp[2] = tr("周二");
    mapInt2Descrp[3] = tr("周三");
    mapInt2Descrp[4] = tr("周四");
    mapInt2Descrp[5] = tr("周五");
    mapInt2Descrp[6] = tr("周六");
    mapInt2Descrp[7] = tr("周日");//Sunday

    foreach(QList<QVariant> workDay,lstStrLstContent)
    {
        //foreach add title
        QString dscrip;
        if (1 == workDay[1].toInt())
        {
            dscrip = "工作日";
        }
        else if (2 == workDay[1].toInt())
        {
            dscrip = "公休日";
        }
        else
        {
            dscrip = "节假日";
        }

        m_lstTitleCollection << QVariant(QString("%1日%2\n%3\n打卡工时").arg(workDay[0].toDate().toString("dd")).arg(mapInt2Descrp[workDay[0].toDate().dayOfWeek()]).arg(dscrip));
        m_lstTitleCollection << QVariant(QString("%1日%2\n%3\n付费工时").arg(workDay[0].toDate().toString("dd")).arg(mapInt2Descrp[workDay[0].toDate().dayOfWeek()]).arg(dscrip));

    }
    //qDebug() << m_lstTitleCollection;

    const int maxRows = m_lstRowLstColumnCollection.size();

    for(int i=0; i<maxRows; ++i)
    {
        if (m_lstRowLstColumnCollection[i].size() == 0)
        {
            continue;
        }

        //引用一行的lst，编辑数据
        QList<QVariant> &lstColumnCollection = m_lstRowLstColumnCollection[i];

        int sumPunchInHours = lstColumnCollection.last().toDouble();
        int needPunchInHours = workDaysNum*8;

        lstColumnCollection << QVariant(workDaysNum*8);

        if (needPunchInHours - sumPunchInHours > 0)
        {
            lstColumnCollection << QVariant(needPunchInHours - sumPunchInHours);
        }
        else
        {
            lstColumnCollection << QVariant(0);
        }



        const QString POID = lstColumnCollection[3].toString();
        const QString ID_number = lstColumnCollection[4].toString();

        QList<QList<QVariant> > lstStrLstContentDutyDetail;

        //timeflag charge_hours
        //timeflag到打卡工时和付费工时的映射
        m_pFpDbProc->getDutyDetailByPOIDIDNumberFromMemDb(lstStrLstContentDutyDetail,POID,ID_number);
        QMap<QDate,QList<double> > mapDate2PunchIn;
        foreach(QList<QVariant> lstDutyDetail,lstStrLstContentDutyDetail)
        {
            mapDate2PunchIn[lstDutyDetail[0].toDate()] << lstDutyDetail[1].toDouble();//punch_hours
            if (int(lstDutyDetail[1].toDouble()) >= 8)
            {
                mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8*lstDutyDetail[3].toInt();//charge_hours
            }
            else
            {
                if (needPunchInHours - sumPunchInHours > 0 || 1 != mapDate2Type[lstDutyDetail[0].toDate()])
                {
                    mapDate2PunchIn[lstDutyDetail[0].toDate()] << lstDutyDetail[2].toDouble();//charge_hours
                }
                else
                {
                    mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8*lstDutyDetail[3].toInt();//charge_hours
                }
            }
        }

        //遍历这个月的日历表，添加汇总表列的明细
        foreach(QList<QVariant> workDay,lstStrLstContent)
        {
            if (mapDate2PunchIn.contains(workDay[0].toDate()))
            {
                lstColumnCollection << QVariant(mapDate2PunchIn[workDay[0].toDate()][0]);//punch_hours
                lstColumnCollection << QVariant(mapDate2PunchIn[workDay[0].toDate()][1]);//charge_hours
            }
            else
            {
                lstColumnCollection << 0;
                lstColumnCollection << 0;
            }
        }
        //qDebug() << lstColumnCollection;
    }

}

void FpDataProc::getDataFromExcel()
{
    m_pFpExcelProc->getDataFromExcel(m_lstTitleDetail,m_lstRowLstColumnDetail);
}

void FpDataProc::setDataIntoExcel()
{

    if (!m_pFpExcelProc->prepareExcel(3))
    {
        return;
    }

    if (0 != m_lstRowLstColumnCollection.size())
    {
        m_pFpExcelProc->setDataIntoExcel(m_lstTitleCollection,m_lstRowLstColumnCollection,1);
    }


    if (0 != m_lstRowLstColumnBelateOrLeaveEarlyDetail.size())
    {
        m_pFpExcelProc->setDataIntoExcel(m_lstTitleDetail,m_lstRowLstColumnBelateOrLeaveEarlyDetail,2);
    }

    if (0 != m_lstRowLstColumnMissPunchInDetail.size())
    {
        m_pFpExcelProc->setDataIntoExcel(m_lstTitleDetail,m_lstRowLstColumnMissPunchInDetail,3);
    }

    m_pFpExcelProc->saveExcel("51");

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

void FpDataProc::getBelateOrLeaveEarlyDetail()
{
    if (!m_pFpDbProc->prepareMemDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->getDutyDetailBelateOrLeaveEarlyFromMemDb(m_lstRowLstColumnBelateOrLeaveEarlyDetail);
}

void FpDataProc::getMissPunchInDetail()
{
    if (!m_pFpDbProc->prepareMemDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->getDutyDetailMissPunchInFromMemDb(m_lstRowLstColumnMissPunchInDetail);
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
    m_lstTitleCollection << tr("公司");
    m_lstTitleCollection << tr("区域");
    m_lstTitleCollection << tr("产品线");
    m_lstTitleCollection << tr("POID");
    m_lstTitleCollection << tr("身份证号码");
    m_lstTitleCollection << tr("姓名");
    m_lstTitleCollection << tr("异常工时");
    m_lstTitleCollection << tr("实际打卡工时");

}

//void FpDataProc::setDutyCollection()
//{
//    if (0 == m_lstRowLstColumnCollection.size())
//    {
//        return;
//    }

//    if (!m_pFpDbProc->prepareMemDb())
//    {
//        qDebug() << "Db Open Failed";
//        return;
//    }
//    m_pFpDbProc->setDutyCollectionIntoMemDb(m_lstRowLstColumnCollection);
//}

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
    return m_pFpDbProc->s_strDetailSQL;
}

const QString &FpDataProc::getDutyDetailBelateOrLeaveEarlySQL()
{
    return m_pFpDbProc->s_strDetailBelateOrLeaveEarlySQL;
}

const QString &FpDataProc::getDutyDetailMissPunchInSQL()
{
    return m_pFpDbProc->s_strDetailMissPunchInSQL;
}

//const QString &FpDataProc::getDutyColletionSQL()
//{
//    return m_pFpDbProc->m_strCollectionSQL;
//}


bool FpDataProc::getAndCheckCurMonth()
{
    QList<QList<QVariant> > lstStrLstContent;
    if (!m_pFpDbProc->getMinMaxMonthFromMemDb(lstStrLstContent))
    {
        return false;
    }

    if (lstStrLstContent.size() != 1 || lstStrLstContent[0].size() != 2)
    {
        return false;
    }

    if (lstStrLstContent[0][0].toDate().toString("yyyyMM")
            == lstStrLstContent[0][1].toDate().toString("yyyyMM"))
    {
        m_strDateMonth = lstStrLstContent[0][0].toDate().toString("yyyyMM");
        //qDebug() << m_strDateMonth;
        return true;
    }

    return false;
}


int FpDataProc::getAbnormalHours(QDateTime dtTimeFlag, QDateTime dtStart, QDateTime dtEnd)
{
    int tmpStart = 0;
    if (dtStart.secsTo(QDateTime(dtTimeFlag.date(),QTime(8,0))) > 0)
    {
        dtStart = QDateTime(dtTimeFlag.date(),QTime(8,0));
    }
    else if (dtStart.secsTo(QDateTime(dtTimeFlag.date(),QTime(12,0))) > 0)
    {
        tmpStart = dtStart.secsTo(QDateTime(dtTimeFlag.date(),QTime(12,0)));
        dtStart = QDateTime(dtTimeFlag.date(),QTime(13,30));
    }
    else if (dtStart.secsTo(QDateTime(dtTimeFlag.date(),QTime(13,30))) > 0)
    {
        dtStart = QDateTime(dtTimeFlag.date(),QTime(13,30));
    }

    int tmpEnd = 0;
    if (QDateTime(dtTimeFlag.date(),QTime(19,0)).secsTo(dtEnd) > 0)
    {
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(19,0));
    }
    else if (QDateTime(dtTimeFlag.date(),QTime(18,0)).secsTo(dtEnd) > 0)
    {
        tmpEnd = QDateTime(dtTimeFlag.date(),QTime(18,0)).secsTo(dtEnd);
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(17,30));
    }
    else if (QDateTime(dtTimeFlag.date(),QTime(17,30)).secsTo(dtEnd) > 0)
    {
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(17,30));
    }
    else if (QDateTime(dtTimeFlag.date(),QTime(13,30)).secsTo(dtEnd) > 0)
    {
        tmpEnd = QDateTime(dtTimeFlag.date(),QTime(13,30)).secsTo(dtEnd);
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(12,0));
    }
    else if (QDateTime(dtTimeFlag.date(),QTime(12,0)).secsTo(dtEnd) > 0)
    {
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(12,00));
    }

    int tempStartEnd = dtStart.secsTo(dtEnd);
    if (tempStartEnd < 0)
    {
        tempStartEnd = 0;
    }

    return (tempStartEnd + tmpStart + tmpEnd);


//    qDebug() << dtStart;
//    qDebug() << dtEnd;
//    qDebug() << "------------";
//    qDebug() << (tempStartEnd)*1.0/3600;
//    qDebug() << (tmpStart)*1.0/3600;
//    qDebug() << (tmpEnd)*1.0/3600;
//    qDebug() << "------------";
//    qDebug() << (tempStartEnd + tmpStart +tmpEnd)*1.0/3600;
}

int FpDataProc::getPunchInHours(QDateTime dtTimeFlag, QDateTime dtStart, QDateTime dtEnd)
{
    int tmpStart = 0;
/*    if (dtStart.secsTo(QDateTime(dtTimeFlag.date(),QTime(8,0))) > 0)
    {
        dtStart = QDateTime(dtTimeFlag.date(),QTime(8,0));
    }
    else */if (dtStart.secsTo(QDateTime(dtTimeFlag.date(),QTime(12,0))) > 0)
    {
        tmpStart = dtStart.secsTo(QDateTime(dtTimeFlag.date(),QTime(12,0)));
        dtStart = QDateTime(dtTimeFlag.date(),QTime(13,30));
    }
    else if (dtStart.secsTo(QDateTime(dtTimeFlag.date(),QTime(13,30))) > 0)
    {
        dtStart = QDateTime(dtTimeFlag.date(),QTime(13,30));
    }

    int tmpEnd = 0;
    if (QDateTime(dtTimeFlag.date(),QTime(18,0)).secsTo(dtEnd) > 0)
    {
        tmpEnd = QDateTime(dtTimeFlag.date(),QTime(18,0)).secsTo(dtEnd);
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(17,30));
    }
    else if (QDateTime(dtTimeFlag.date(),QTime(17,30)).secsTo(dtEnd) > 0)
    {
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(17,30));
    }
    else if (QDateTime(dtTimeFlag.date(),QTime(13,30)).secsTo(dtEnd) > 0)
    {
        tmpEnd = QDateTime(dtTimeFlag.date(),QTime(13,30)).secsTo(dtEnd);
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(12,0));
    }
    else if (QDateTime(dtTimeFlag.date(),QTime(12,0)).secsTo(dtEnd) > 0)
    {
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(12,00));
    }

    int tempStartEnd = dtStart.secsTo(dtEnd);
    if (tempStartEnd < 0)
    {
        tempStartEnd = 0;
    }

    return (tempStartEnd + tmpStart + tmpEnd);


//    qDebug() << dtStart;
//    qDebug() << dtEnd;
//    qDebug() << "------------";
//    qDebug() << (tempStartEnd)*1.0/3600;
//    qDebug() << (tmpStart)*1.0/3600;
//    qDebug() << (tmpEnd)*1.0/3600;
//    qDebug() << "------------";
//    qDebug() << (tempStartEnd + tmpStart +tmpEnd)*1.0/3600;
}

int FpDataProc::getOverTimeHours(QDateTime dtTimeFlag, QDateTime dtStart, QDateTime dtEnd)
{
    int tmpStart = 0;
//    if (dtStart.secsTo(QDateTime(dtTimeFlag.date(),QTime(12,0))) > 0)
//    {
//        tmpStart = dtStart.secsTo(QDateTime(dtTimeFlag.date(),QTime(12,0)));
//        dtStart = QDateTime(dtTimeFlag.date(),QTime(13,30));
//    }
//    else if (dtStart.secsTo(QDateTime(dtTimeFlag.date(),QTime(13,30))) > 0)
//    {
//        dtStart = QDateTime(dtTimeFlag.date(),QTime(13,30));
//    }

    int tmpEnd = 0;
//    if (QDateTime(dtTimeFlag.date(),QTime(13,30)).secsTo(dtEnd) > 0)
//    {
//        tmpEnd = QDateTime(dtTimeFlag.date(),QTime(13,30)).secsTo(dtEnd);
//        dtEnd = QDateTime(dtTimeFlag.date(),QTime(12,0));
//    }
//    else if (QDateTime(dtTimeFlag.date(),QTime(12,0)).secsTo(dtEnd) > 0)
//    {
//        dtEnd = QDateTime(dtTimeFlag.date(),QTime(12,00));
//    }

    int tempStartEnd = dtStart.secsTo(dtEnd);
    if (tempStartEnd < 0)
    {
        tempStartEnd = 0;
    }

    return (tempStartEnd + tmpStart + tmpEnd);


//    qDebug() << dtStart;
//    qDebug() << dtEnd;
//    qDebug() << "------------";
//    qDebug() << (tempStartEnd)*1.0/3600;
//    qDebug() << (tmpStart)*1.0/3600;
//    qDebug() << (tmpEnd)*1.0/3600;
//    qDebug() << "------------";
//    qDebug() << (tempStartEnd + tmpStart +tmpEnd)*1.0/3600;
}
