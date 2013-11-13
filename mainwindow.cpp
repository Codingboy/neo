#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wordpool.h"
#include <cstdio>

MainWindow::MainWindow(Statistic* stats, int lektion, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->type->init(this->ui->display, lektion, stats, 3, this, ui->keyboard);
}

MainWindow::~MainWindow()
{
    delete ui;
}
