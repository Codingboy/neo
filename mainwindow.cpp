#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wordpool.h"
#include <cstdio>
#include <QDir>
#include <QDebug>

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
        qDebug() << files.at(i);
        QAction* action = new QAction(files.at(i), this);
        lessons.append(action);
        lesson->addAction(action);
    }
    QMenu* statistic = this->ui->menuBar->addMenu("&Statistic");
    QMenu* about = this->ui->menuBar->addMenu("&About");
    this->stats = new Statistic();
    ui->type->preinit(this->ui->display, stats, 300, this, ui->keyboard);
    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
    for (int i=0; i<files.size(); i++)
    {
        connect(lessons.at(i), SIGNAL(triggered()), ui->type, SLOT(init()));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
