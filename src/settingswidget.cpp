#include "settingswidget.h"
#include "ui_settingswidget.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    this->settings = new QSettings("settings.ini", QSettings::IniFormat);
    if (this->settings->value("blockOnError").toBool())
    {
        this->ui->blockOnError->setChecked(true);
    }
    if (this->settings->value("playErrorSound").toBool())
    {
        this->ui->playErrorSound->setChecked(true);
    }
    this->ui->fontSize->setValue(this->settings->value("fontSize").toInt());
    this->ui->fontBoldSize->setValue(this->settings->value("fontBoldSize").toInt());
    connect(this->ui->ok, SIGNAL(clicked()), this, SLOT(ok()));
    connect(this->ui->cancel, SIGNAL(clicked()), this, SLOT(cancel()));
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
    delete this->settings;
}

void SettingsWidget::ok()
{
    this->settings->setValue("blockOnError", this->ui->blockOnError->isChecked());
    this->settings->setValue("playErrorSound", this->ui->playErrorSound->isChecked());
    this->settings->setValue("fontSize", this->ui->fontSize->value());
    this->settings->setValue("fontBoldSize", this->ui->fontBoldSize->value());
    ((QFrame*)(this->parent()))->close();
}

void SettingsWidget::cancel()
{
    ((QFrame*)(this->parent()))->close();
}
