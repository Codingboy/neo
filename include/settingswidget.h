#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

private:
    Ui::SettingsWidget *ui;
    QSettings* settings;
public slots:
    void ok();
    void cancel();
};

#endif // SETTINGSWIDGET_H
