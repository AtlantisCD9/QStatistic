#ifndef DIALOGIMPORTXLS_H
#define DIALOGIMPORTXLS_H

#include <QDialog>

namespace Ui {
class DialogImportXls;
}

class DialogImportXls : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogImportXls(QWidget *parent = 0);
    ~DialogImportXls();

    bool getInfo(QString &inPutFile, int &titleEnd, int &sheetID, int &columnNum);
    
private:
    Ui::DialogImportXls *ui;

private slots:
    void onGetOpenFileName();
};

#endif // DIALOGIMPORTXLS_H
