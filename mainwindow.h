#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class FpDataProc;
class QTableView;

namespace Ui {
class MainWindow;
}

class DialogImportXls;
class DialogMergeXls;
class DialogExportXls;
class DialogStatistics;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    DialogImportXls *m_importXls;
    DialogMergeXls *m_mergeXls;
    DialogExportXls *m_exportXls;
    DialogStatistics *m_statistics;

    QAction *m_actionImportXls;
    QAction *m_actionMergeXls;
    QAction *m_actionExportXls;
    QAction *m_actionStatistics;
    QMenu *m_menuFile;

    QAction *m_actionWorkDaysSetting;
    QMenu *m_menuConfig;

    QAction *m_actionAbout;
    QMenu *m_menuHelp;

    FpDataProc *m_pFpDataProc;

private:
    void initialMenuFile();
    void initialMenuConfig();
    void initialMenuHelp();

    void showDetail(QTableView *tableWidget);
    void showDetailBelateOrLeaveEarly(QTableView *tableWidget);
    void showDetailMissPunchIn(QTableView *tableWidget);
    void showCollection(QTableView *tableWidget);


private slots:
    void onImportFile();
    void onMergeFile();
    void onExportFile();
    void onStatistics();

    void onAbout();

};

#endif // MAINWINDOW_H
