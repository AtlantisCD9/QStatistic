#ifndef DIALOGEXPORTXLS_H
#define DIALOGEXPORTXLS_H

#include <QDialog>

namespace Ui {
class DialogExportXls;
}

class DialogExportXls : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogExportXls(QWidget *parent = 0);
    ~DialogExportXls();

    bool getInfo(QString &outPutFile, int &sheetID);
    
private:
    Ui::DialogExportXls *ui;

private slots:
    void onGetSaveFileName();
};

#endif // DIALOGEXPORTXLS_H
