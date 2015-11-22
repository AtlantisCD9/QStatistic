#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;

    QAction *m_actionImportXls;
    QAction *m_actionExportXls;
    QMenu *m_menuFile;

    QAction *m_actionWorkDaysSetting;
    QMenu *m_menuConfig;

    QAction *m_actionAbout;
    QMenu *m_menuHelp;

    void procData();
    void getDataFromExcel();
    void setDataIntoExcel();

    bool prepareDb();
    bool getDataFromDb();
    bool setDataIntoDb();

private slots:
    void onImportFile();
    void onExportFile();

    void onAbout();

};

#endif // MAINWINDOW_H
