#ifndef FPDBPROC_H
#define FPDBPROC_H

#include <QObject>

class DbOper;

class FpDbProc : public QObject
{
    Q_OBJECT
public:
    ~FpDbProc();
    static FpDbProc *getInstance(QObject *parent = 0);
    static void releaseInstance();

    bool prepareMemDb();
    bool getDataFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    bool setDataIntoMemDb(QList<QList<QVariant> > &lstStrLstContent);

    bool prepareLocalDb();

private:
    explicit FpDbProc(QObject *parent = 0);

private:
    static FpDbProc *m_pInstance;

    DbOper *m_pDbOperMem;
    DbOper *m_pDbOperLocal;

};

#endif // FPDBPROC_H
