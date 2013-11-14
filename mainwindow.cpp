#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wordpool.h"
#include <cstdio>
#include <QDir>
#include <QDebug>
#include "statisticwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMenu* file = this->ui->menuBar->addMenu("&File");
    QAction* quit = new QAction("&Quit", this);
    file->addAction(quit);
    QMenu* lesson = this->ui->menuBar->addMenu("&Lesson");
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
    }
    QMenu* statistic = this->ui->menuBar->addMenu("&Statistic");
    QAction* show = new QAction("&Show", this);
    statistic->addAction(show);
    QAction* reset = new QAction("&Reset", this);
    statistic->addAction(reset);
    QMenu* aboutMenu = this->ui->menuBar->addMenu("&About");
    QAction* aboutAction = new QAction("&About", this);
    aboutMenu->addAction(aboutAction);
    this->stats = new Statistic();
    ui->type->preinit(this->ui->display, stats, 10, this, ui->keyboard);
    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(show, SIGNAL(triggered()), this, SLOT(openStatistic()));
    connect(reset, SIGNAL(triggered()), this, SLOT(resetStatistic()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));
    for (int i=0; i<files.size(); i++)
    {
        connect(lessons.at(i), SIGNAL(triggered()), ui->type, SLOT(init()));
    }
}

void MainWindow::openStatistic()
{
    StatisticWindow* w = new StatisticWindow();
    w->show();
}

void MainWindow::resetStatistic()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Reset Statistic");
    msgBox.setText("Do you really want to reset the complete statistic?");
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
    QMessageBox::about(this, "About", "Author:\tDaniel Tkocz\nContact:\tdaniel.tkocz@gmx.de\nLicense:\tBSD 2-Clause\n\ninspired by:\n\thttps://online.tipp10.com\n\thttp://neo-layout.org");
}

MainWindow::~MainWindow()
{
    delete ui;
}
