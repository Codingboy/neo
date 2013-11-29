#include "settingswidget.h"
#include "ui_settingswidget.h"
#include <QDebug>

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
    this->ui->pow->setValue(this->settings->value("pow").toFloat());
    if (this->settings->value("visualErrorFeedback").toBool())
    {
        this->ui->visualErrorFeedback->setChecked(true);
    }
    this->ui->goodLineA->setValue(this->settings->value("goodLineA").toInt());
    this->ui->badLineA->setValue(this->settings->value("badLineA").toInt());
    this->ui->goodLineE->setValue(this->settings->value("goodLineE").toFloat());
    this->ui->badLineE->setValue(this->settings->value("badLineE").toFloat());
    this->ui->influencingSessions->setValue(this->settings->value("influencingSessions").toInt());
    this->ui->timeout->setValue(this->settings->value("timeout").toInt());
    this->ui->sessionDuration->setValue(this->settings->value("sessionDuration").toInt()/60);
    connect(this->ui->ok, SIGNAL(clicked()), this, SLOT(ok()));
    connect(this->ui->cancel, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(this->ui->goodLineA, SIGNAL(valueChanged(int)), this, SLOT(adjustBadLineA()));
    connect(this->ui->goodLineE, SIGNAL(valueChanged(double)), this, SLOT(adjustBadLineE()));
    connect(this->ui->badLineA, SIGNAL(valueChanged(int)), this, SLOT(adjustGoodLineA()));
    connect(this->ui->badLineE, SIGNAL(valueChanged(double)), this, SLOT(adjustGoodLineE()));
}

void SettingsWidget::adjustBadLineA()
{
    if (((QSpinBox*)sender())->value() <= this->ui->badLineA->value())
    {
        this->ui->badLineA->setValue(((QSpinBox*)sender())->value()-1);
    }
}

void SettingsWidget::adjustGoodLineA()
{
    if (((QSpinBox*)sender())->value() >= this->ui->goodLineA->value())
    {
        this->ui->goodLineA->setValue(((QSpinBox*)sender())->value()+1);
    }
}

void SettingsWidget::adjustBadLineE()
{
    if (((QDoubleSpinBox*)sender())->value() >= this->ui->badLineE->value())
    {
        this->ui->badLineE->setValue(((QDoubleSpinBox*)sender())->value()+0.1);
    }
}

void SettingsWidget::adjustGoodLineE()
{
    if (((QDoubleSpinBox*)sender())->value() <= this->ui->goodLineE->value())
    {
        this->ui->goodLineE->setValue(((QDoubleSpinBox*)sender())->value()-0.1);
    }
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
    this->settings->setValue("pow", this->ui->pow->value());
    this->settings->setValue("visualErrorFeedback", this->ui->visualErrorFeedback->isChecked());
    this->settings->setValue("goodLineA", this->ui->goodLineA->value());
    this->settings->setValue("badLineA", this->ui->badLineA->value());
    this->settings->setValue("goodLineE", this->ui->goodLineE->value());
    this->settings->setValue("badLineE", this->ui->badLineE->value());
    this->settings->setValue("influencingSessions", this->ui->influencingSessions->value());
    this->settings->setValue("sessionDuration", this->ui->sessionDuration->value()*60);
    this->settings->setValue("timeout", this->ui->timeout->value());
    ((QFrame*)(this->parent()))->close();
    qDebug() << "saved settings";
}

void SettingsWidget::cancel()
{
    ((QFrame*)(this->parent()))->close();
}
