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
    m_lstTitleDetail.clear();//��ϸ̧ͷ
    m_lstTitleCollection.clear();//����̧ͷ

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

                int itemp = int(secsTemp*1.0/3600*100);
                int ihead = itemp/100;
                int itail = itemp%100;
                double dAbnormalHour = 0.0;
                if (0 == itemp)
                {
                    dAbnormalHour = 0;
                }
                else if (itail > 50)
                {
                    dAbnormalHour = ihead+1;
                }
                else
                {
                    dAbnormalHour = ihead+0.5;
                }
                m_lstRowLstColumnDetail[i] << QVariant(dAbnormalHour);
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
    //����Ƿ���ĳ����
    if (!getAndCheckCurMonth())
    {
        return;
    }

    //�����ĳ���£���ô��ȡ���·ݵ�������
    QList<QList<QVariant> > lstStrLstContent;
    m_pFpDbProc->getWorkDaysByCurMonthFromMemDb(lstStrLstContent,m_strDateMonth);

    QMap<QDate,int> mapDate2Type;
    foreach(QList<QVariant> workDay,lstStrLstContent)
    {
        mapDate2Type[workDay[0].toDate()] = workDay[1].toInt();
    }

    m_lstTitleCollection << tr("�쳣��ʱ");//6
    m_lstTitleCollection << tr("ʵ�ʴ򿨹�ʱ");//7
    m_lstTitleCollection << tr("Ӧ����ʱ");//8
    m_lstTitleCollection << tr("Ƿ��ʱ");//9
    m_lstTitleCollection << tr("�Ӱ๤ʱ");//10

    int workDaysNum = 0;//���¹�����
    foreach(QList<QVariant> workDay,lstStrLstContent)
    {
        if (1 == workDay[1].toInt())
        {
            ++workDaysNum;
        }
    }

    //�����쳣��ʱ����ȡ����
    const int abnormalHourID = 6;
    //����򿨹�ʱ��ȥ���ڼ���
    const int sumPunchInHourID = 7;
    const int needPunchInHourID = 8;
    const int oweHourID = 9;
    const int overHourID = 10;



    //��ʼ��dayOfWeek����������ӳ��
    QMap<int,QString> mapInt2Descrp;
    mapInt2Descrp[1] = tr("��һ");//Monday
    mapInt2Descrp[2] = tr("�ܶ�");
    mapInt2Descrp[3] = tr("����");
    mapInt2Descrp[4] = tr("����");
    mapInt2Descrp[5] = tr("����");
    mapInt2Descrp[6] = tr("����");
    mapInt2Descrp[7] = tr("����");//Sunday

    foreach(QList<QVariant> workDay,lstStrLstContent)
    {
        //foreach add title
        QString dscrip;
        if (1 == workDay[1].toInt())
        {
            dscrip = "������";
        }
        else if (2 == workDay[1].toInt())
        {
            dscrip = "������";
        }
        else
        {
            dscrip = "�ڼ���";
        }

        m_lstTitleCollection << QVariant(QString("%1��%2\n%3\n�򿨹�ʱ").arg(workDay[0].toDate().toString("dd")).arg(mapInt2Descrp[workDay[0].toDate().dayOfWeek()]).arg(dscrip));
        m_lstTitleCollection << QVariant(QString("%1��%2\n%3\n���ѹ�ʱ").arg(workDay[0].toDate().toString("dd")).arg(mapInt2Descrp[workDay[0].toDate().dayOfWeek()]).arg(dscrip));

    }
    //qDebug() << m_lstTitleCollection;

    const int maxRows = m_lstRowLstColumnCollection.size();

    for(int i=0; i<maxRows; ++i)
    {
        if (m_lstRowLstColumnCollection[i].size() == 0)
        {
            continue;
        }

        //����һ�е�lst���༭����
        QList<QVariant> &lstColumnCollection = m_lstRowLstColumnCollection[i];
        const QString POID = lstColumnCollection[3].toString();
        const QString ID_number = lstColumnCollection[4].toString();

        //�쳣��ʱ
        QList<QList<QVariant> > lstStrLstContentDutyAbnormal;
        //round(sum(abnormal_hours),2)
        m_pFpDbProc->getDutyAbnormalHoursSumByPOIDIDNumberFromMemDb(lstStrLstContentDutyAbnormal,POID,ID_number);
        if (lstStrLstContentDutyAbnormal.size() > 0)
        {
            lstColumnCollection << lstStrLstContentDutyAbnormal.first()[0].toDouble();
        }
        else
        {
            lstColumnCollection << QVariant(0);
        }

        //�������մ�
        QList<QList<QVariant> > lstStrLstContentDutyPunchIn;
        //round(sum(punch_hours),2)
        m_pFpDbProc->getDutyPunchInHoursSumByPOIDIDNumberFromMemDb(lstStrLstContentDutyPunchIn,POID,ID_number);
        if (lstStrLstContentDutyPunchIn.size() > 0)
        {
            lstColumnCollection << lstStrLstContentDutyPunchIn.first()[0].toDouble();
        }
        else
        {
            lstColumnCollection << QVariant(0);
        }

        QList<QList<QVariant> > lstStrLstContentDutyDetail;
        //timeflag,round(punch_hours,2),round(punch_hours*payroll_multi,2) AS charge_h,payroll_multi
        m_pFpDbProc->getDutyDetailByPOIDIDNumberFromMemDb(lstStrLstContentDutyDetail,POID,ID_number);

        //��ȡ�д򿨼�¼������
        QList<QDate> lstDatePunchIn;
        foreach(QList<QVariant> lstDutyDetail,lstStrLstContentDutyDetail)
        {
            lstDatePunchIn << lstDutyDetail[0].toDate();
        }
        //���������·ݣ���û�д����ڵ�����Ϊ�쳣��ʱ
        //��������µ���������ӻ��ܱ��е���ϸ
        foreach(QList<QVariant> workDay,lstStrLstContent)
        {
            if (!lstDatePunchIn.contains(workDay[0].toDate()))
            {
                //����ǹ����գ�����û�м�¼����ô��Ҫ�����쳣��ʱ
                if (1 == workDay[1].toInt())
                {
                    double abnormalHour = lstColumnCollection[abnormalHourID].toDouble();
                    abnormalHour += 8.0;
                    lstColumnCollection[abnormalHourID] = QVariant(abnormalHour);
                }
            }
        }

        //Ӧ����ʱ
        lstColumnCollection << QVariant(workDaysNum*8-lstColumnCollection[abnormalHourID].toDouble());
        double needPunchInHours = lstColumnCollection[needPunchInHourID].toDouble();
//        qDebug() << lstColumnCollection[5];
//        qDebug() << workDaysNum*8;
//        qDebug() << workDaysNum*8-lstColumnCollection[abnormalHourID].toDouble();
//        qDebug() << needPunchInHours;

        //Ƿ��ʱ
        double sumPunchInHours = lstColumnCollection[sumPunchInHourID].toDouble();
        if (needPunchInHours - sumPunchInHours > 0)
        {
            lstColumnCollection << QVariant(needPunchInHours - sumPunchInHours);
        }
        else
        {
            lstColumnCollection << QVariant(0);
        }

        //�Ӱ๤ʱ
        QList<QList<QVariant> > lstStrLstContentDutyOverHours;
        //round(sum(punch_hours),2)
        m_pFpDbProc->getDutyOverHoursSumByPOIDIDNumberFromMemDb(lstStrLstContentDutyOverHours,POID,ID_number);
        if (lstStrLstContentDutyOverHours.size() > 0)
        {
            lstColumnCollection << lstStrLstContentDutyOverHours.first()[0].toDouble();
        }
        else
        {
            lstColumnCollection << QVariant(0);
        }





        //timeflag���򿨹�ʱ�͸��ѹ�ʱ��ӳ��
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

        //��������µ���������ӻ��ܱ��е���ϸ
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
    m_lstTitleCollection << tr("��˾");
    m_lstTitleCollection << tr("����");
    m_lstTitleCollection << tr("��Ʒ��");
    m_lstTitleCollection << tr("POID");
    m_lstTitleCollection << tr("���֤����");
    m_lstTitleCollection << tr("����");
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
    m_pFpDbProc->updateDutyOverHours();
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
