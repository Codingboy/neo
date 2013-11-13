#ifndef STATISTICWINDOW_H
#define STATISTICWINDOW_H
#include "ui_statisticwindow.h"
#include <QMainWindow>

namespace Ui {
class StatisticWindow;
}

class StatisticWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit StatisticWindow(QWidget *parent = 0);

private:
    Ui::StatisticWindow* ui;
signals:

public slots:

};

#endif // STATISTICWINDOW_H
