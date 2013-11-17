#ifndef STATISTICWINDOW_H
#define STATISTICWINDOW_H

#include <QWidget>

namespace Ui {
class StatisticWidget;
}

class StatisticWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StatisticWidget(QWidget *parent = 0);

private:
    Ui::StatisticWidget* ui;
signals:

public slots:

};

#endif // STATISTICWINDOW_H
