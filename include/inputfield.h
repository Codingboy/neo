#ifndef INPUTFIELD_H
#define INPUTFIELD_H

#include <QTextEdit>
#include "wordpool.h"
#include "statistic.h"
#include <QTime>
#include <QMainWindow>
#include <QLabel>
#include <QSettings>
#include <QSound>
#include <QProcess>
#include <QLabel>

class InputField : public QTextEdit
{
    Q_OBJECT
public:
    explicit InputField(QObject *parent = 0);
    ~InputField();
    void preinit(QTextEdit* display, Statistic* stats, unsigned int time, QMainWindow* mw, QLabel* keyboard, QLabel* timeLeftLabel, QLabel* hitsLabel, QLabel* mistakesLabel, QLabel* hitsPerMinuteLabel, QLabel* mistakesRateLabel);
    unsigned int mistakes;
    unsigned int corrects;
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    QTextEdit* display;
    void showText();
    Wordpool* words;
    bool errorstate;
    Statistic* stats;
    unsigned int time;
    QTime startTime;
    QMainWindow* mw;
    QPixmap neo1;
    QPixmap neo2;
    QPixmap neo3;
    QLabel* keyboard;
    Wordpool* loadWordpool(QString& lesson);
    QSettings* settings;
    QString lesson;
    QSound* sound;
#ifdef _WIN32
    QProcess* neo20;
#endif
    int fontSize;
    int fontBoldSize;
    QLabel* timeLeftLabel;
    QLabel* hitsLabel;
    QLabel* mistakesLabel;
    QLabel* hitsPerMinuteLabel;
    QLabel* mistakesRateLabel;
    
signals:
    
public slots:
    void abort();
    void init();
    
};

#endif // INPUTFIELD_H
