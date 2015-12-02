#ifndef FPDBPROC_H
#define FPDBPROC_H

#include <QObject>

class DbOper;

class FpDbProc : public QObject
{
    Q_OBJECT
public:
    explicit FpDbProc(QObject *parent = 0);
    ~FpDbProc();

    bool prepareMemDb();
    bool prepareLocalDb();

    bool getDutyDistinctPersonalFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    bool setDutyCollectionIntoMemDb(QList<QList<QVariant> > &lstStrLstContent);
    bool setDutyDetailIntoMemDb(QList<QList<QVariant> > &lstStrLstContent);


    bool setWorkDaysIntoMemDb(QList<QList<QVariant> > &lstStrLstContent);
    bool setWorkDaysIntoLocalDb(QList<QList<QVariant> > &lstStrLstContent);
    bool getWorkDaysFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    bool getWorkDaysFromLocalDb(QList<QList<QVariant> > &lstStrLstContent);

    bool getWorkDaysByCurMonthFromMemDb(QList<QList<QVariant> > &lstStrLstContent,const QString &strCurMonth);
    bool getMinMaxMonthFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    bool getDutyDetailByPOIDIDNumberFromMemDb(QList<QList<QVariant> > &lstStrLstContent,
                                          const QString &POID, const QString &IDNumber);


public:
    const static QString m_strDetailSQL;
    const static QString m_strDetailBelateOrLeaveEarlySQL;
    const static QString m_strDetailMissPunchInSQL;
    const static QString m_strCollectionSQL;

private:
    bool getWorkDaysFromDb(QList<QList<QVariant> > &lstStrLstContent,DbOper *argDbOper);
    bool setWorkDaysIntoDb(QList<QList<QVariant> > &lstStrLstContent,DbOper *argDbOper);


private:
    DbOper *m_pDbOperMem;
    DbOper *m_pDbOperLocal;

    bool m_bIsMemPrepared;
    bool m_bIsLocalPrepared;

};

#endif // FPDBPROC_H
