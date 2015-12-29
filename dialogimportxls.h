#ifndef DIALOGIMPORTXLS_H
#define DIALOGIMPORTXLS_H

#include <QDialog>
#include "globaldef.h"

namespace Ui {
class DialogImportXls;
}

class DialogImportXls : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogImportXls(QWidget *parent = 0);
    ~DialogImportXls();

    bool getInfo(QString &inPutFile, int &titleEnd, int &sheetID, int &columnNum, ENUM_IMPORT_XLS_TYPE &type);
    
private:
    Ui::DialogImportXls *ui;

private slots:
    void onGetOpenFileName();
    void onRefresh();
};

#endif // DIALOGIMPORTXLS_H
