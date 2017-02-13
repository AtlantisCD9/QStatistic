#include <QCoreApplication>
#include <QTextCodec>
#include <QVariant>
#include <QList>
#include <QDate>
#include <QDebug>

#include "fpdbproc.h"
#include "dboper.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    app.addLibraryPath("./plugins");

    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForTr(codec);
    //QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);

    FpDbProc *m_pFpDbProc = new FpDbProc;

    QList<QList<QVariant> > lstLstContentWorkDays;
    QMap<int,int> mapWeek2Type;
    mapWeek2Type[1] = 1;
    mapWeek2Type[2] = 1;
    mapWeek2Type[3] = 1;
    mapWeek2Type[4] = 1;
    mapWeek2Type[5] = 1;
    mapWeek2Type[6] = 2;
    mapWeek2Type[7] = 2;

    int yearNum = 2017;
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
        return -1;
    }
    m_pFpDbProc->setWorkDaysIntoLocalDb(lstLstContentWorkDays);

    return 0;
}
