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
    bool getDataFromMemDb(QList<QList<QVariant> > &lstStrLstContent);
    bool setDataIntoMemDb(QList<QList<QVariant> > &lstStrLstContent);

    bool prepareLocalDb();

private:
    DbOper *m_pDbOperMem;
    DbOper *m_pDbOperLocal;

};

#endif // FPDBPROC_H
