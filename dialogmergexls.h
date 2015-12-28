#ifndef DIALOGMERGEXLS_H
#define DIALOGMERGEXLS_H

#include <QDialog>
#include <QStringList>
#include <QString>

namespace Ui {
class DialogMergeXls;
}

class DialogMergeXls : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogMergeXls(QWidget *parent = 0);
    ~DialogMergeXls();

    bool getInfo(QStringList &lstMergeFile, QString &outPutFile, int &titleEnd, int &sheetID, int &columnNum);
    
private:
    Ui::DialogMergeXls *ui;

private slots:
    void onGetOpenFileNames();
    void onGetSaveFileName();
};

#endif // DIALOGMERGEXLS_H
