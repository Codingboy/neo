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

void Wordpool::finalise()
{
    this->stats->setUsedWords(&(this->words));
}

void Wordpool::removeWord(QString& word)
{
}

const QString& Wordpool::getRandomWord()
{
    return stats->getRecommendedWord();
}
