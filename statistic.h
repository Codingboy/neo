#ifndef STATISTIC_H
#define STATISTIC_H

#include <QMap>
#include <QPair>
#include <QString>
#include <QList>

class Statistic
{
public:
    Statistic();
    void load(unsigned int number);
    void save(unsigned int number);
    void reportSuccess(const QChar& prev, const QChar& actual, const QChar& next);
    void reportMistake(const QChar& prev, const QChar& actual, const QChar& next);
    const QString& getRecommendedWord();
    void setUsedWords(QList<QString>* words);
private:
    QMap<QString, QPair<unsigned int, unsigned int> >* stats;//first=successes, second=mistakes
    QList<QPair<QString, float> >* sorted;//sorted by value
};

#endif // STATISTIC_H
