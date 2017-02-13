#-------------------------------------------------
#
# Project created by QtCreator 2015-11-14T01:37:46
#
#-------------------------------------------------

QT       -= gui
QT       += core sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = createCalendar
TEMPLATE = app


SOURCES += createCalendar.cpp\
    ../dboper.cpp \
    ../fpdbproc.cpp \

HEADERS  += ../dboper.h \
    ../fpdbproc.h \

INCLUDEPATH += ../