#include "fpdataproc.h"

#include "fpdbproc.h"
#include "fpexcelproc.h"

#include "globaldef.h"

#include <QStringList>
#include <QMap>
#include <QDate>
#include <QDateTime>
#include <QVariant>
#include <QDebug>
#include <QMessageBox>

FpDataProc::FpDataProc(QObject *parent) :
    QObject(parent)
{
    m_pFpDbProc = new FpDbProc;
    m_pFpExcelProc = new FpExcelProc;

    //createWorkDays();
    getWorkDays();
    syncWorkDays();
}

FpDataProc::~FpDataProc()
{
    delete m_pFpDbProc;
    delete m_pFpExcelProc;
}

void FpDataProc::initial(ENUM_IMPORT_XLS_TYPE importType)
{
    switch(importType)
    {
    case IM_DETAIL:
        m_lstTitleDetail.clear();//明细抬头
        m_lstRowLstColumnDetail.clear();//will be add some proc data
        m_pFpDbProc->initDutyDetailMemDb();
        break;
    case IM_ABNORMAL:
        m_lstTitleAbnormal.clear();//明细抬头
        m_lstRowLstColumnAbnormal.clear();//will be add some proc data
        m_pFpDbProc->initProcAbnormalDetailMemDb();
        break;
    case IM_PO_SWITCH:
        m_lstTitlePOSwitch.clear();//明细抬头
        m_lstRowLstColumnPOSwitch.clear();//will be add some proc data
        m_pFpDbProc->initPoSwitchMemDb();
        break;
    case IM_STATISTICS:
        m_lstTitleCollection.clear();//汇总抬头
        m_lstRowLstColumnBelateOrLeaveEarlyDetail.clear();//will be add some proc data
        m_lstRowLstColumnMissPunchInDetail.clear();//will be add some proc data
        m_lstRowLstColumnCollection.clear();//for month collection will be add some proc data
        m_pFpDbProc->initDutyPersonalSumMemDb();
        break;
    default:
        break;
    }



}

double FpDataProc::getAbnormalHourUnit(int secsTemp)
{
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

    return dAbnormalHour;
}

QDateTime FpDataProc::getTimeFlag(QDateTime dtStart)
{
    QDateTime dtTimeFlag;
    if (dtStart.time() >= QTime(5,1))
    {
        dtTimeFlag = QDateTime(dtStart.date(),QTime(0,0));
    }
    else
    {
        dtTimeFlag = QDateTime(dtStart.date().addDays(-1),QTime(0,0));
    }

    return dtTimeFlag;
}

int FpDataProc::getPunchType(QDateTime dtTimeFlag, QDateTime dtStart, QDateTime dtEnd)
{
    //punch_type:0,normal;1,beLate or leaveEarly;2,abnormal;
    int punchType;
    if (dtEnd.isNull())
    {
        punchType = PUNCH_ABNORMAL;//abnormal
    }
    else
    {
        if(dtStart >= QDateTime(dtTimeFlag.date(),QTime(9,1))
                || dtEnd < QDateTime(dtTimeFlag.date(),QTime(17,30)))
        {
           punchType = PUNCH_BELATE_OR_LEAVEEARLY;//beLate or leaveEarly
        }
        else
        {
            punchType = PUNCH_NORMAL;//normal
        }
    }

    return punchType;
}

double FpDataProc::getAbnormalHourFNL(QDateTime dtTimeFlag, QDateTime dtStart, QDateTime dtEnd,
                                   int punchType, int payrollMulti)
{
    double abnormalHour;

    if (punchType == int(PUNCH_ABNORMAL))
    {
        abnormalHour = 0;//打卡异常的异常工时此处统一设置成0，在汇总处理时统一刷新

    }
    else if (punchType == int(PUNCH_BELATE_OR_LEAVEEARLY))
    {
        if (1 == payrollMulti)
        {
            int secsTemp = getAbnormalHours(dtTimeFlag,dtStart,dtEnd);
            abnormalHour = getAbnormalHourUnit(secsTemp);
        }
        else
        {
            abnormalHour = 0;
        }
    }
    else
    {
        abnormalHour = 0;
    }

    return abnormalHour;
}

double FpDataProc::getPunchHour(QDateTime dtTimeFlag, QDateTime dtStart, QDateTime dtEnd, int payrollMulti)
{
    double punchHour;
    if (!dtEnd.isNull())
    {
        if (1 == payrollMulti)
        {
            punchHour = getPunchInHours(dtTimeFlag,dtStart,dtEnd)*1.0/3600;
        }
        else
        {
            int overTimeHours = 8*3600;
            if (getOverTimeHours(dtTimeFlag,dtStart,dtEnd) < overTimeHours)
            {
                overTimeHours = getOverTimeHours(dtTimeFlag,dtStart,dtEnd);
            }
            punchHour = overTimeHours*1.0/3600;
        }
    }
    else
    {
        punchHour = 0.0;
    }

    return punchHour;
}


void FpDataProc::procDataForDatail()
{
    //上班时间
    const int onDutyID = 7;
    //下班时间
    const int offDutyID = 8;

    QMap<QDate,int> mapDate2Type;
    foreach(QList<QVariant> workDay,m_lstRowLstColumnWorkDays)
    {
        mapDate2Type[workDay[0].toDate()] = workDay[1].toInt();
    }

    //QList<QVariant> lstTitle;
    for(int i=0;i<m_lstRowLstColumnDetail.size();++i)
    {
        QDateTime dtStart = m_lstRowLstColumnDetail[i][onDutyID].toDateTime();
        QDateTime dtEnd = m_lstRowLstColumnDetail[i][offDutyID].toDateTime();
        QDateTime dtTimeFlag = getTimeFlag(dtStart);

        //timeflag
        m_lstRowLstColumnDetail[i] << QVariant(dtTimeFlag);

        //day_of_week
        m_lstRowLstColumnDetail[i] << QVariant(dtTimeFlag.date().dayOfWeek());

        //punch_type:0,normal;1,beLate or leaveEarly;2,abnormal;
        m_lstRowLstColumnDetail[i] << QVariant(getPunchType(dtTimeFlag,dtStart,dtEnd));

        //abnormal_hours
        m_lstRowLstColumnDetail[i] << QVariant(getAbnormalHourFNL(dtTimeFlag,dtStart,dtEnd,
                                                                  m_lstRowLstColumnDetail[i].last().toInt(),
                                                                  mapDate2Type[dtTimeFlag.date()]));

        //punch_hours
        m_lstRowLstColumnDetail[i] << QVariant(getPunchHour(dtTimeFlag,dtStart,dtEnd,
                                                            mapDate2Type[dtTimeFlag.date()]));

        //payroll_multi
        m_lstRowLstColumnDetail[i] << QVariant(mapDate2Type[dtTimeFlag.date()]);

        //charge_hours
        m_lstRowLstColumnDetail[i] << QVariant(0.0);
    }
}

void FpDataProc::procDataAbnormal()
{
    //po号码
    const int poID = 0;
    //姓名
    const int name = 1;
    //上班时间
    const int onDutyID = 3;
    //下班时间
    const int offDutyID = 4;


    QMap<QDate,int> mapDate2Type;
    foreach(QList<QVariant> workDay,m_lstRowLstColumnWorkDays)
    {
        mapDate2Type[workDay[0].toDate()] = workDay[1].toInt();
    }

    //QList<QVariant> lstTitle;
    for(int i=0;i<m_lstRowLstColumnAbnormal.size();++i)
    {
        QDateTime dtStart = m_lstRowLstColumnAbnormal[i][onDutyID].toDateTime();
        QDateTime dtEnd = m_lstRowLstColumnAbnormal[i][offDutyID].toDateTime();
        QDateTime dtTimeFlag = getTimeFlag(dtStart);

        if(dtTimeFlag.date() != dtStart.date() || dtTimeFlag.date() != dtEnd.date())
        {
            QMessageBox::information(0,"ERROR",
                                     QString("异常工时处理表单，存在开始和结束时间不是同一天\nPO号码：%1\n姓名：%2\n该条记录不能被处理")
                                     .arg(m_lstRowLstColumnAbnormal[i][poID].toString())
                                     .arg(m_lstRowLstColumnAbnormal[i][name].toString()));
            break;
        }

        //timeflag
        m_lstRowLstColumnAbnormal[i] << QVariant(dtTimeFlag);

        //punch_type:0,normal;1,beLate or leaveEarly;2,abnormal;
        m_lstRowLstColumnAbnormal[i] << QVariant(getPunchType(dtTimeFlag,dtStart,dtEnd));

        //abnormal_hours
        m_lstRowLstColumnAbnormal[i] << QVariant(getAbnormalHourFNL(dtTimeFlag,dtStart,dtEnd,
                                                                  m_lstRowLstColumnAbnormal[i].last().toInt(),
                                                                  mapDate2Type[dtTimeFlag.date()]));

        //punch_hours
        m_lstRowLstColumnAbnormal[i] << QVariant(getPunchHour(dtTimeFlag,dtStart,dtEnd,
                                                            mapDate2Type[dtTimeFlag.date()]));
    }

}

void FpDataProc::procDataForCollection()
{
    //检查是否处于某个月
    QDate startTemp;
    QDate endTemp;
    if (!getAndCheckCurMonth(startTemp,endTemp))
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

    m_lstTitleCollection << tr("异常工时");//6
    m_lstTitleCollection << tr("实际打卡工时");//7
    m_lstTitleCollection << tr("应服务工时");//8
    m_lstTitleCollection << tr("欠工时");//9
    m_lstTitleCollection << tr("加班工时");//10

    const int POID_ID = 3;
    const int ID_number_ID = 4;

    QDate startDate;
    QDate endDate;

    //处理异常工时——取整数
    const int abnormalHourID = 6;
    //处理打卡工时，去除节假日
    const int sumPunchInHourID = 7;
    const int needPunchInHourID = 8;
    const int oweHourID = 9;
    //const int overHourID = 10;



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
            dscrip = "工作日x1";
        }
        else if (2 == workDay[1].toInt())
        {
            dscrip = "公休日x2";
        }
        else
        {
            dscrip = "节假日x3";
        }

        //m_lstTitleCollection << QVariant(QString("%1日%2\n%3\n打卡工时").arg(workDay[0].toDate().toString("dd")).arg(mapInt2Descrp[workDay[0].toDate().dayOfWeek()]).arg(dscrip));
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
        const QString POID = lstColumnCollection[POID_ID].toString();
        const QString ID_number = lstColumnCollection[ID_number_ID].toString();

        //月结汇总项目起止时间
        QList<QList<QVariant> > lstStrLstContentDate;
        //start_date,end_date
        m_pFpDbProc->getDutyPersDateByPoIDIDNumberFromMemDb(lstStrLstContentDate,
                                                            POID,ID_number);
        if (lstStrLstContentDate.size() > 0)
        {
            startDate = lstStrLstContentDate.first()[0].toDate();
            endDate = lstStrLstContentDate.first()[1].toDate();
        }

        //异常工时
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

        //处理工作日打卡
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
        //timeflag,round(punch_hours,2),round(abnormal_hours,2)
        m_pFpDbProc->getDutyDetailByPOIDIDNumberFromMemDb(lstStrLstContentDutyDetail,POID,ID_number);

        //获取有打卡记录的日期
        QList<QDate> lstDatePunchIn;
        foreach(QList<QVariant> lstDutyDetail,lstStrLstContentDutyDetail)
        {
            lstDatePunchIn << lstDutyDetail[0].toDate();
        }
        //遍历整个月份，把没有打卡日期的设置为异常工时
        //遍历这个月的日历表，添加汇总表列的明细
        foreach(QList<QVariant> workDay,lstStrLstContent)
        {
            if (!lstDatePunchIn.contains(workDay[0].toDate()))
            {
                //如果是工作日，但是没有记录，那么需要加入异常工时#Atlantis#
                if (1 == workDay[1].toInt()
                        && workDay[0].toDate() >= startDate
                        && workDay[0].toDate() <= endDate)
                {
                    double abnormalHour = lstColumnCollection[abnormalHourID].toDouble();
                    abnormalHour += 8.0;
                    lstColumnCollection[abnormalHourID] = QVariant(abnormalHour);
                }
            }
        }

        //应服务工时
        //此处，应服务工时需要结合出项入项数据#Atlantis#
        int workDaysNum = 0;//本月工作日
        foreach(QList<QVariant> workDay,lstStrLstContent)
        {
            if (1 == workDay[1].toInt()
                    && workDay[0].toDate() >= startDate
                    && workDay[0].toDate() <= endDate)
            {
                ++workDaysNum;
            }
        }
        lstColumnCollection << QVariant(workDaysNum*8-lstColumnCollection[abnormalHourID].toDouble());
        double needPunchInHours = lstColumnCollection[needPunchInHourID].toDouble();
//        qDebug() << lstColumnCollection[5];
//        qDebug() << workDaysNum*8;
//        qDebug() << workDaysNum*8-lstColumnCollection[abnormalHourID].toDouble();
//        qDebug() << needPunchInHours;

        //欠工时
        double sumPunchInHours = lstColumnCollection[sumPunchInHourID].toDouble();
        if (int(100*(needPunchInHours - sumPunchInHours)) > 0)
        {
            lstColumnCollection << QVariant(needPunchInHours - sumPunchInHours);
        }
        else
        {
            lstColumnCollection << QVariant(0);
        }

        //加班工时
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





        //timeflag到打卡工时和付费工时的映射
        QMap<QDate,QList<double> > mapDate2PunchIn;
        double dMoreHours = 0.0;
        double dOweHours = lstColumnCollection[oweHourID].toDouble();
        //qDebug() << lstColumnCollection[5];
        foreach(QList<QVariant> lstDutyDetail,lstStrLstContentDutyDetail)
        {
            mapDate2PunchIn[lstDutyDetail[0].toDate()] << lstDutyDetail[1].toDouble();//punch_hours
//            qDebug() << lstDutyDetail[0].toDate();
//            qDebug() << lstDutyDetail[1].toDouble();
//            qDebug() << lstDutyDetail[2].toDouble();

            if (1 == mapDate2Type[lstDutyDetail[0].toDate()])//工作日
            {
                if (int(100*lstDutyDetail[2].toDouble()) > 0)//当天存在异常工时
                {
                    mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8 - lstDutyDetail[2].toDouble();
                }
                else
                {
                    if (int(100*dOweHours) == 0)//不欠工时，一律填8小时
                    {
                        mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8;//charge_hours
                    }
                    else//欠工时，就将多出8小时的工时填补欠工时数，当欠工时为0后，一律填8小时，走上一个case
                    {
                        if (int(lstDutyDetail[1].toDouble()) >= 8)//满工时
                        {
                            mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8;//charge_hours
                            dMoreHours += lstDutyDetail[1].toDouble() - 8;
                        }
                        else//欠工时
                        {
                            if (int(lstDutyDetail[1].toDouble()+dMoreHours) >= 8)//多余工时够补欠工时
                            {
                                mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8;//charge_hours
                                dMoreHours += lstDutyDetail[1].toDouble() - 8;
                            }
                            else//多余工时不够补欠工时
                            {
                                if (int(lstDutyDetail[1].toDouble()+dMoreHours+dOweHours) >= 8)//算上欠工时后，仍然欠工时，则仅补多余工时
                                {
                                    mapDate2PunchIn[lstDutyDetail[0].toDate()] << lstDutyDetail[1].toDouble()+dMoreHours;
                                    dOweHours += lstDutyDetail[1].toDouble()+dMoreHours-8;
                                }
                                else//算上欠工时后，仍然欠工时，则补多余工时且补足欠工时数，后续一律补齐8小时
                                {
                                    mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8 - dOweHours;
                                    dOweHours = 0;
                                }
                                dMoreHours = 0;
                            }
                        }
                    }
                }
            }
            else//非工作日
            {
                if (int(lstDutyDetail[1].toDouble()) >= 8)//满工时
                {
                    mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8;//charge_hours
                }
                else
                {
                    mapDate2PunchIn[lstDutyDetail[0].toDate()] << lstDutyDetail[1].toDouble();//charge_hours;
                }
            }
        }

        //遍历这个月的日历表，添加汇总表列的明细
        foreach(QList<QVariant> workDay,lstStrLstContent)
        {
            if (mapDate2PunchIn.contains(workDay[0].toDate()))
            {
                //lstColumnCollection << QVariant(mapDate2PunchIn[workDay[0].toDate()][0]);//punch_hours
                lstColumnCollection << QVariant(mapDate2PunchIn[workDay[0].toDate()][1]);//charge_hours
            }
            else
            {
                //此处，应服务工时需要结合出项入项数据#Atlantis#
                if (workDay[0].toDate() >= startDate && workDay[0].toDate() <= endDate)
                {
                    lstColumnCollection << 0;
                }
                else
                {
                    lstColumnCollection << QVariant();
                }
            }
        }
        //qDebug() << lstColumnCollection;
    }

}

bool FpDataProc::getDataFromExcel(QString &inPutFile, int &titleEnd, int &sheetID, int &columnNum, ENUM_IMPORT_XLS_TYPE import_type)
{
    switch(import_type)
    {
    case IM_DETAIL:
        m_pFpExcelProc->getDataFromExcel(inPutFile,m_lstTitleDetail,m_lstRowLstColumnDetail,
                                         titleEnd,sheetID,columnNum);
        break;
    case IM_ABNORMAL:
        m_pFpExcelProc->getDataFromExcel(inPutFile,m_lstTitleAbnormal,m_lstRowLstColumnAbnormal,
                                         titleEnd,sheetID,columnNum);
        break;
    case IM_PO_SWITCH:
        m_pFpExcelProc->getDataFromExcel(inPutFile,m_lstTitlePOSwitch,m_lstRowLstColumnPOSwitch,
                                         titleEnd,sheetID,columnNum);
        break;
    default:
        break;
    }

    return true;
}

void FpDataProc::mergeExcel(QStringList &lstMergeFile, QString &outPutFile, int &titleEnd, int &sheetID, int &columnNum)
{
    //lstMergeFile,outputFile,headEnd,sheetID
    QList<QVariant> lstTitleDetail;//明细抬头
    QList<QList<QVariant> > lstRowLstColumnDetail;//will be add some proc data

//    QStringList lstFileName;
//    if(!m_pFpExcelProc->getExcelOpenFileList(lstFileName))
//    {
//        return;
//    }

//    QString saveFileName;
//    if(!m_pFpExcelProc->getExcelSaveFile(saveFileName))
//    {
//        return;
//    }

    foreach(QString fileName,lstMergeFile)
    {
        m_pFpExcelProc->getDataFromExcel(fileName,lstTitleDetail,lstRowLstColumnDetail,titleEnd,sheetID,columnNum);
//        foreach(QList<QVariant> lstColumn,lstRowLstColumnDetail)
//        {
//            qDebug() << lstColumn;
//        }
    }
    //////-----------------

    if (!m_pFpExcelProc->prepareExcel(EX_MERGE_TOTAL))
    {
        return;
    }

    if (0 != lstRowLstColumnDetail.size())
    {
        m_pFpExcelProc->setDataIntoExcel(lstTitleDetail,lstRowLstColumnDetail);
    }

    m_pFpExcelProc->saveExcel(outPutFile,"51");
}

void FpDataProc::setDataIntoExcel(QString &outPutFile, int &sheetID)
{
    if (!m_pFpExcelProc->prepareExcel(EX_MONTH_TOTAL,sheetID+2))
    {
        return;
    }

    if (0 != m_lstRowLstColumnCollection.size())
    {
        m_pFpExcelProc->setDataIntoExcel(m_lstTitleCollection,m_lstRowLstColumnCollection,1,sheetID);
    }


    if (0 != m_lstRowLstColumnBelateOrLeaveEarlyDetail.size())
    {
        m_pFpExcelProc->setDataIntoExcel(m_lstTitleDetail,m_lstRowLstColumnBelateOrLeaveEarlyDetail,1,sheetID+1);
    }

    if (0 != m_lstRowLstColumnMissPunchInDetail.size())
    {
        m_pFpExcelProc->setDataIntoExcel(m_lstTitleDetail,m_lstRowLstColumnMissPunchInDetail,1,sheetID+2);
    }

    m_pFpExcelProc->saveExcel(outPutFile,"51");

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

    int yearNum = 2016;
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

void FpDataProc::getDutyPersonalSum()
{
    if (!m_pFpDbProc->prepareMemDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->getDutyPersonalSumFromMemDb(m_lstRowLstColumnCollection);
    //poid,job_id,name,SUM(punch_hours)
    m_lstTitleCollection << tr("公司");
    m_lstTitleCollection << tr("区域");
    m_lstTitleCollection << tr("产品线");
    m_lstTitleCollection << tr("POID");
    m_lstTitleCollection << tr("身份证号码");
    m_lstTitleCollection << tr("姓名");
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

void FpDataProc::updateDutyDetailByProcAbnormalDetail()
{
    m_pFpDbProc->updateDutyDetailByProcAbnormalDetail();
}

void FpDataProc::updateDutyPersonalSumByDutyDetailMemDb()
{
    m_pFpDbProc->updateDutyPersonalSumByDutyDetailMemDb();
}

void FpDataProc::updateDutyPersonalSumSetDateInterMemDb(QDate startDate, QDate endDate)
{
    m_pFpDbProc->updateDutyPersonalSumSetDateInterMemDb(startDate,endDate);
}

void FpDataProc::updateDutyPersonalSumByPoSwitchMemDb()
{
    m_pFpDbProc->updateDutyPersonalSumByPoSwitchMemDb();
}

void FpDataProc::setProcAbnormalDetail()
{
    if (0 == m_lstRowLstColumnAbnormal.size())
    {
        return;
    }

    if (!m_pFpDbProc->prepareMemDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->setProcAbnormalDetailIntoMemDb(m_lstRowLstColumnAbnormal);
}

void FpDataProc::setPoSwtich()
{
    if (0 == m_lstRowLstColumnPOSwitch.size())
    {
        return;
    }

    if (!m_pFpDbProc->prepareMemDb())
    {
        qDebug() << "Db Open Failed";
        return;
    }
    m_pFpDbProc->setPoSwitchIntoMemDb(m_lstRowLstColumnPOSwitch);
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


bool FpDataProc::getAndCheckCurMonth(QDate &startDate, QDate &endDate)
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
        startDate = lstStrLstContent[0][0].toDate();
        endDate = lstStrLstContent[0][1].toDate();
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
    /*if (QDateTime(dtTimeFlag.date(),QTime(19,0)).secsTo(dtEnd) > 0)
    {
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(19,0));
    }
    else if (QDateTime(dtTimeFlag.date(),QTime(18,0)).secsTo(dtEnd) > 0)
    {
        tmpEnd = QDateTime(dtTimeFlag.date(),QTime(18,0)).secsTo(dtEnd);
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(17,30));
    }
    else */if (QDateTime(dtTimeFlag.date(),QTime(17,30)).secsTo(dtEnd) > 0)
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
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(12,0));
    }

    int tempStartEnd = dtStart.secsTo(dtEnd);
    if (tempStartEnd < 0)
    {
        tempStartEnd = 0;
    }

    return (8*3600-(tempStartEnd + tmpStart + tmpEnd));


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
        dtEnd = QDateTime(dtTimeFlag.date(),QTime(12,0));
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
    Q_UNUSED(dtTimeFlag);
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
//        dtEnd = QDateTime(dtTimeFlag.date(),QTime(12,0));
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
