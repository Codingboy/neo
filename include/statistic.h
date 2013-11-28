#ifndef STATISTIC_H
#define STATISTIC_H

#include <QMap>
#include <QPair>
#include <QString>
#include <QList>
#include <QTime>

class Statistic
{
public:
    Statistic();
    virtual ~Statistic();
    void save(unsigned int number, unsigned int corrects, unsigned int mistakes, QString& lesson);
    void reportSuccess(const QChar& prevprev, const QChar& prev, const QChar& actual, const QChar& next, const QChar& nextnext);
    void reportMistake(const QChar& prevprev, const QChar& prev, const QChar& actual, const QChar& next, const QChar& nextnext);
    const QString& getRecommendedWord();
    void setUsedWords(QList<QString>* words);
    void load();
    void timeout();
private:
    float expRand();
    void load(unsigned int number);
    QMap<QString, QPair<unsigned int, unsigned int> >* stats;//first=successes, second=mistakes
    QMap<QString, QList<unsigned int> >* timeStats;
    QList<QPair<QString, float> >* sorted;//sorted by value
    QTime lastSuccess;
    bool timeoutOccured;
};

#endif // STATISTIC_H
