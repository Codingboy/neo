#-------------------------------------------------
#
# Project created by QtCreator 2013-11-11T10:09:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = neo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    wordpool.cpp \
    inputfield.cpp \
    statistic.cpp

HEADERS  += mainwindow.h \
    wordpool.h \
    inputfield.h \
    statistic.h

FORMS    += mainwindow.ui
