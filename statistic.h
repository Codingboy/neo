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
    void load();
    void save();
    void reportSuccess(const QChar& prev, const QChar& actual, const QChar& next);
    void reportMistake(const QChar& prev, const QChar& actual, const QChar& next);
    const QString& getRecommendedWord(QList<QString> words);
private:
    QMap<QString, QPair<unsigned int, unsigned int> > stats;//first=successes, second=mistakes
    QList<QPair<QString, float> > sorted;//sorted by value
    void sort();
};

#endif // STATISTIC_H
