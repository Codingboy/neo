#ifndef WORDPOOL_H
#define WORDPOOL_H

#include <QString>
#include <QList>
#include "statistic.h"

class Wordpool
{
    public:
        Wordpool(Statistic* stats);
        void addWord(QString& word);
        void removeWord(QString& word);
        const QString& getRandomWord();
    private:
        QList<QString> words;
        Statistic* stats;
};

#endif // WORDPOOL_H
