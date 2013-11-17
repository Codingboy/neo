#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "wordpool.h"
#include "statistic.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow* ui;
    Statistic* stats;
public slots:
    void openStatistic();
    void resetStatistic();
    void showAbout();
    void showSettings();
    void resetSettings();
};

#endif // MAINWINDOW_H
