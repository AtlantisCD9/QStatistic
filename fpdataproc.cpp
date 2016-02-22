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
        m_lstTitleDetail.clear();//��ϸ̧ͷ
        m_lstRowLstColumnDetail.clear();//will be add some proc data
        m_pFpDbProc->initDutyDetailMemDb();
        break;
    case IM_ABNORMAL:
        m_lstTitleAbnormal.clear();//��ϸ̧ͷ
        m_lstRowLstColumnAbnormal.clear();//will be add some proc data
        m_pFpDbProc->initProcAbnormalDetailMemDb();
        break;
    case IM_PO_SWITCH:
        m_lstTitlePOSwitch.clear();//��ϸ̧ͷ
        m_lstRowLstColumnPOSwitch.clear();//will be add some proc data
        m_pFpDbProc->initPoSwitchMemDb();
        break;
    case IM_STATISTICS:
        m_lstTitleCollection.clear();//����̧ͷ
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
        abnormalHour = 0;//���쳣���쳣��ʱ�˴�ͳһ���ó�0���ڻ��ܴ���ʱͳһˢ��

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
    //�ϰ�ʱ��
    const int onDutyID = 7;
    //�°�ʱ��
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
    //po����
    const int poID = 0;
    //����
    const int name = 1;
    //�ϰ�ʱ��
    const int onDutyID = 3;
    //�°�ʱ��
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
                                     QString("�쳣��ʱ����������ڿ�ʼ�ͽ���ʱ�䲻��ͬһ��\nPO���룺%1\n������%2\n������¼���ܱ�����")
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
    //����Ƿ���ĳ����
    QDate startTemp;
    QDate endTemp;
    if (!getAndCheckCurMonth(startTemp,endTemp))
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

    const int POID_ID = 3;
    const int ID_number_ID = 4;

    QDate startDate;
    QDate endDate;

    //�����쳣��ʱ����ȡ����
    const int abnormalHourID = 6;
    //����򿨹�ʱ��ȥ���ڼ���
    const int sumPunchInHourID = 7;
    const int needPunchInHourID = 8;
    const int oweHourID = 9;
    //const int overHourID = 10;



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
            dscrip = "������x1";
        }
        else if (2 == workDay[1].toInt())
        {
            dscrip = "������x2";
        }
        else
        {
            dscrip = "�ڼ���x3";
        }

        //m_lstTitleCollection << QVariant(QString("%1��%2\n%3\n�򿨹�ʱ").arg(workDay[0].toDate().toString("dd")).arg(mapInt2Descrp[workDay[0].toDate().dayOfWeek()]).arg(dscrip));
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
        const QString POID = lstColumnCollection[POID_ID].toString();
        const QString ID_number = lstColumnCollection[ID_number_ID].toString();

        //�½������Ŀ��ֹʱ��
        QList<QList<QVariant> > lstStrLstContentDate;
        //start_date,end_date
        m_pFpDbProc->getDutyPersDateByPoIDIDNumberFromMemDb(lstStrLstContentDate,
                                                            POID,ID_number);
        if (lstStrLstContentDate.size() > 0)
        {
            startDate = lstStrLstContentDate.first()[0].toDate();
            endDate = lstStrLstContentDate.first()[1].toDate();
        }

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
        //timeflag,round(punch_hours,2),round(abnormal_hours,2)
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
                //����ǹ����գ�����û�м�¼����ô��Ҫ�����쳣��ʱ#Atlantis#
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

        //Ӧ����ʱ
        //�˴���Ӧ����ʱ��Ҫ��ϳ�����������#Atlantis#
        int workDaysNum = 0;//���¹�����
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

        //Ƿ��ʱ
        double sumPunchInHours = lstColumnCollection[sumPunchInHourID].toDouble();
        if (int(100*(needPunchInHours - sumPunchInHours)) > 0)
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
        double dMoreHours = 0.0;
        double dOweHours = lstColumnCollection[oweHourID].toDouble();
        //qDebug() << lstColumnCollection[5];
        foreach(QList<QVariant> lstDutyDetail,lstStrLstContentDutyDetail)
        {
            mapDate2PunchIn[lstDutyDetail[0].toDate()] << lstDutyDetail[1].toDouble();//punch_hours
//            qDebug() << lstDutyDetail[0].toDate();
//            qDebug() << lstDutyDetail[1].toDouble();
//            qDebug() << lstDutyDetail[2].toDouble();

            if (1 == mapDate2Type[lstDutyDetail[0].toDate()])//������
            {
                if (int(100*lstDutyDetail[2].toDouble()) > 0)//��������쳣��ʱ
                {
                    mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8 - lstDutyDetail[2].toDouble();
                }
                else
                {
                    if (int(100*dOweHours) == 0)//��Ƿ��ʱ��һ����8Сʱ
                    {
                        mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8;//charge_hours
                    }
                    else//Ƿ��ʱ���ͽ����8Сʱ�Ĺ�ʱ�Ƿ��ʱ������Ƿ��ʱΪ0��һ����8Сʱ������һ��case
                    {
                        if (int(lstDutyDetail[1].toDouble()) >= 8)//����ʱ
                        {
                            mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8;//charge_hours
                            dMoreHours += lstDutyDetail[1].toDouble() - 8;
                        }
                        else//Ƿ��ʱ
                        {
                            if (int(lstDutyDetail[1].toDouble()+dMoreHours) >= 8)//���๤ʱ����Ƿ��ʱ
                            {
                                mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8;//charge_hours
                                dMoreHours += lstDutyDetail[1].toDouble() - 8;
                            }
                            else//���๤ʱ������Ƿ��ʱ
                            {
                                if (int(lstDutyDetail[1].toDouble()+dMoreHours+dOweHours) >= 8)//����Ƿ��ʱ����ȻǷ��ʱ����������๤ʱ
                                {
                                    mapDate2PunchIn[lstDutyDetail[0].toDate()] << lstDutyDetail[1].toDouble()+dMoreHours;
                                    dOweHours += lstDutyDetail[1].toDouble()+dMoreHours-8;
                                }
                                else//����Ƿ��ʱ����ȻǷ��ʱ���򲹶��๤ʱ�Ҳ���Ƿ��ʱ��������һ�ɲ���8Сʱ
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
            else//�ǹ�����
            {
                if (int(lstDutyDetail[1].toDouble()) >= 8)//����ʱ
                {
                    mapDate2PunchIn[lstDutyDetail[0].toDate()] << 8;//charge_hours
                }
                else
                {
                    mapDate2PunchIn[lstDutyDetail[0].toDate()] << lstDutyDetail[1].toDouble();//charge_hours;
                }
            }
        }

        //��������µ���������ӻ��ܱ��е���ϸ
        foreach(QList<QVariant> workDay,lstStrLstContent)
        {
            if (mapDate2PunchIn.contains(workDay[0].toDate()))
            {
                //lstColumnCollection << QVariant(mapDate2PunchIn[workDay[0].toDate()][0]);//punch_hours
                lstColumnCollection << QVariant(mapDate2PunchIn[workDay[0].toDate()][1]);//charge_hours
            }
            else
            {
                //�˴���Ӧ����ʱ��Ҫ��ϳ�����������#Atlantis#
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
    QList<QVariant> lstTitleDetail;//��ϸ̧ͷ
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
