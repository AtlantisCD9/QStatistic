#-------------------------------------------------
#
# Project created by QtCreator 2015-11-14T01:37:46
#
#-------------------------------------------------

QT       += core gui\
            sql

CONFIG += qaxcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fpStatistic
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        qexcel.cpp \
    dboper.cpp \
    fpdbproc.cpp \
    fpexcelproc.cpp \
    fpdataproc.cpp

HEADERS  += mainwindow.h\
            qexcel.h \
    dboper.h \
    fpdbproc.h \
    fpexcelproc.h \
    fpdataproc.h

FORMS    += mainwindow.ui
