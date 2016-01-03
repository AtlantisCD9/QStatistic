#ifndef DIALOGSTATISTICS_H
#define DIALOGSTATISTICS_H

#include <QDialog>
#include <QDate>

namespace Ui {
class DialogStatistics;
}

class DialogStatistics : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogStatistics(QWidget *parent = 0);
    ~DialogStatistics();

    void setInfo(int detail,int abnormal,int poSwitch);

    void setDate(QDate &startDate,QDate &endDate);

    bool getInfo(QDate &startDate,QDate &endDate);
    
private:
    Ui::DialogStatistics *ui;

};

#endif // DIALOGSTATISTICS_H
