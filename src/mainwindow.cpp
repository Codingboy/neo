#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wordpool.h"
#include <cstdio>
#include <QDir>
#include <QDebug>
#include "statisticwidget.h"
#include <QMessageBox>
#include <QFrame>
#include "settingswidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMenu* file = this->ui->menuBar->addMenu("&Datei");
    QAction* quit = new QAction("&Schließen", this);
    file->addAction(quit);
    QAction* abort = new QAction("&Abbrechen", this);
    file->addAction(abort);
    QMenu* lesson = this->ui->menuBar->addMenu("&Übung");
    QList<QAction*> lessons;
    QDir dir("wp");
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    dir.setNameFilters(QStringList("*.wp"));
    QStringList files = dir.entryList();
    for (int i=0; i<files.size(); i++)
    {
        QAction* action = new QAction(files.at(i), this);
        lessons.append(action);
        lesson->addAction(action);
        qDebug() << "added lesson" << files.at(i);
    }
    QMenu* statistic = this->ui->menuBar->addMenu("&Statistik");
    QAction* show = new QAction("&Anzeigen", this);
    statistic->addAction(show);
    QAction* reset = new QAction("&Zurücksetzen", this);
    statistic->addAction(reset);
    QMenu* settingsMenu = this->ui->menuBar->addMenu("&Einstellungen");
    QAction* settingsAction = new QAction("&Ändern", this);
    settingsMenu->addAction(settingsAction);
    QAction* resetSettingsAction = new QAction("&Zurücksetzen", this);
    settingsMenu->addAction(resetSettingsAction);
    QMenu* aboutMenu = this->ui->menuBar->addMenu("&Über");
    QAction* aboutAction = new QAction("&Über", this);
    aboutMenu->addAction(aboutAction);
    this->stats = new Statistic();
    ui->type->preinit(this->ui->display, stats, this, ui->keyboard, ui->timeLeft, ui->hits, ui->mistakes, ui->hitsPerMinute, ui->mistakesper100);
    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(show, SIGNAL(triggered()), this, SLOT(openStatistic()));
    connect(reset, SIGNAL(triggered()), this, SLOT(resetStatistic()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    connect(resetSettingsAction, SIGNAL(triggered()), this, SLOT(resetSettings()));
    connect(abort, SIGNAL(triggered()), this->ui->type, SLOT(abort()));
    for (int i=0; i<files.size(); i++)
    {
        connect(lessons.at(i), SIGNAL(triggered()), ui->type, SLOT(init()));
    }
}

void MainWindow::showSettings()
{
    QFrame* f = new QFrame();
    SettingsWidget* w = new SettingsWidget(f);
    f->setWindowTitle("Einstellungen");
    f->setMinimumSize(w->size());
    f->show();
}

void MainWindow::openStatistic()
{
    QFrame* f = new QFrame();
    StatisticWidget* w = new StatisticWidget(f);
    f->setWindowTitle("Statistik");
    f->setMinimumSize(w->size());
    f->show();
}

void MainWindow::resetStatistic()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Statistik zurücksetzen");
    msgBox.setText("Möchten Sie wirklich die gesamte Statistik zurücksetzen?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() == QMessageBox::Yes)
    {
        QDir dir("wp");
        dir.setFilter(QDir::Files);
        dir.setSorting(QDir::Name);
        dir.setNameFilters(QStringList("*.stats"));
        QStringList files = dir.entryList();
        for (int i=0; i<files.size(); i++)
        {
            QFile del(dir.absoluteFilePath(files.at(i)));
            del.remove();
        }
    }
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About", "Autor:\tDaniel Tkocz\nKontakt:\tdaniel.tkocz@gmx.de\nLizenz:\tBSD 2-Clause\n\ninspiriert von:\n\thttps://online.tipp10.com\n\thttp://neo-layout.org");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resetSettings()
{
    QSettings s("settings.ini", QSettings::IniFormat);
    s.setValue("blockOnError", true);
    s.setValue("playErrorSound", true);
    s.setValue("fontSize", 20);
    s.setValue("goodLineA", 120);
    s.setValue("goodLineE", 2);
    s.setValue("badLineA", 100);
    s.setValue("badLineE", 3);
    s.setValue("pow", 2);
    s.setValue("visualErrorFeedback", true);
    s.setValue("influencingSessions", 5);
    s.setValue("sessionDuration", 300);
    s.setValue("timeout", 3000);
    qDebug() << "settings resetted";
}
