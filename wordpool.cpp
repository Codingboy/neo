#include "wordpool.h"

Wordpool::Wordpool(Statistic* stats)
{
    this->stats = stats;
}

void Wordpool::addWord(QString& word)
{
    if (!this->words.contains(word))
    {
        this->words.append(word);
    }
}

void Wordpool::removeWord(QString& word)
{
}

QList<QString>& Wordpool::getWords()
{
    return this->words;
}

const QString& Wordpool::getRandomWord()
{
    return stats->getRecommendedWord(this->words);
}
