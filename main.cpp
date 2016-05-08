#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.addLibraryPath("./plugins");

    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForTr(codec);
    //QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);

    MainWindow win;
    win.showMaximized();
    
    return app.exec();
}
