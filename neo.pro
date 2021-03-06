#-------------------------------------------------
#
# Project created by QtCreator 2013-11-13T19:13:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = neo
TEMPLATE = app

INCLUDEPATH += include
CONFIG += c++11

# Input
HEADERS += include/inputfield.h include/mainwindow.h include/statistic.h include/wordpool.h \
    include/statisticwidget.h \
    include/settingswidget.h
FORMS += form/mainwindow.ui \
    form/statisticwidget.ui \
    form/settingswidget.ui
SOURCES += src/inputfield.cpp src/main.cpp src/mainwindow.cpp src/statistic.cpp src/wordpool.cpp \
    src/statisticwidget.cpp \
    src/settingswidget.cpp
