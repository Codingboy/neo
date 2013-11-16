#-------------------------------------------------
#
# Project created by QtCreator 2013-11-13T19:13:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = neo
TEMPLATE = app


# Input
HEADERS += inputfield.h mainwindow.h statistic.h wordpool.h \
    statisticwidget.h
FORMS += mainwindow.ui \
    statisticwidget.ui
SOURCES += inputfield.cpp main.cpp mainwindow.cpp statistic.cpp wordpool.cpp \
    statisticwidget.cpp
