#include <QApplication>
#include "mainwindow.h"
#include "wordpool.h"
#include "statistic.h"
#include <cstdio>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Statistic stats;
    MainWindow w(&stats, 1);
    w.show();
    
    return app.exec();
}
