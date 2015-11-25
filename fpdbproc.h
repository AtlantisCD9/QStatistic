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
    bool getDutyCollectionFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    bool setDutyDetailIntoMemDb(QList<QList<QVariant> > &lstStrLstContent);

    bool prepareLocalDb();
    bool getWorkDaysFromLocalDb(QList<QList<QVariant> > &lstStrLstContent);
    bool setWorkDaysIntoLocalDb(QList<QList<QVariant> > &lstStrLstContent);

private:
    DbOper *m_pDbOperMem;
    DbOper *m_pDbOperLocal;

    bool m_bIsMemPrepared;
    bool m_bIsLocalPrepared;

};

#endif // FPDBPROC_H
