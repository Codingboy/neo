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
#include <QTimer>
#include <QElapsedTimer>

class InputField : public QTextEdit
{
    Q_OBJECT
public:
    explicit InputField(QObject *parent = 0);
    ~InputField();
    void preinit(QTextEdit* display, Statistic* stats, QMainWindow* mw, QLabel* keyboard, QLabel* timeLeftLabel, QLabel* hitsLabel, QLabel* mistakesLabel, QLabel* hitsPerMinuteLabel, QLabel* mistakesRateLabel);
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
    QLabel* timeLeftLabel;
    QLabel* hitsLabel;
    QLabel* mistakesLabel;
    QLabel* hitsPerMinuteLabel;
    QLabel* mistakesRateLabel;
    QTimer* timeoutTimer;
    QTimer* guiUpdateTimer;
    QTimer* sessionTimer;
    bool timeout;
    bool firstKeyPress;
    int timeUntilEnd;
    QElapsedTimer* elapsed;
    int correctTextLength;
    bool running;

signals:
    void correctTextTyped();
    void scrollDisplay2(int);
    
public slots:
    void handleTimeout();
    void handleSessionEnd();
    void handleGuiUpdate();
    void abort();
    void init();
    void scrollDisplay();
};

#endif // INPUTFIELD_H
