#include "statistic.h"
#include <QTime>
#include <cstdio>
#include <QMap>
#include <climits>
#include <QFile>
#include <QList>
#include <QDir>
#include <QDebug>

Statistic::Statistic()
{
    this->sorted = new QList<QPair<QString, float> >();
    this->stats = new QMap<QString, QPair<unsigned int, unsigned int> >();
    qsrand(QTime::currentTime().msec());
    load(0);
}

void Statistic::load(unsigned int number)
{
    QString path = QString("stats")+QDir::separator()+QString::number(number, 10)+QString(".stats");
    QFile in(path);
    if (!in.exists())
    {
        qDebug() << "file \"" << path << "\" does not exist";
        return;
    }
    if (!in.open(QIODevice::ReadOnly))
    {
        qDebug() << "file \"" << path << "\" not openable";
        return;
    }
    while (true)
    {
        QByteArray bytes = in.readLine();
        if (bytes.size() == 0)
        {
            break;
        }
        QString line(bytes);
        line.remove("\n");
        line.remove("\r");
        QStringList sl = line.split(" ");
        unsigned int successes = sl.at(0).toInt();
        unsigned int mistakes = sl.at(1).toInt();
        QString key;
        for (int i=2; i<sl.size(); i++)
        {
            key += QChar(sl.at(i).toInt());
        }
        if (this->stats->contains(key))
        {
            successes += this->stats->value(key).first;
            mistakes += this->stats->value(key).second;
        }
        this->stats->insert(key, qMakePair(successes, mistakes));
    }
    in.close();
}

void Statistic::save(unsigned int number)
{
    QString path = QString("stats")+QDir::separator()+QString::number(number, 10)+QString(".stats");
	QFile out(path);
    if (out.exists())
    {
        qDebug() << "file \"" << path << "\" already exists";
    }
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "file \"" << path << "\" not openable";
        return;
    }
    QList<QString> keys = this->stats->keys();
    QString str;
    for (int i=0; i<keys.size(); i++)
    {
        unsigned int successes = this->stats->value(keys.at(i)).first;
        unsigned int mistakes = this->stats->value(keys.at(i)).second;
        str.setNum(successes);
        out.write(str.toStdString().c_str());
        out.write(" ");
        str.setNum(mistakes);
        out.write(str.toStdString().c_str());
        QString key = keys.at(i);
        for (int i=0; i<key.length(); i++)
        {
            int tmp = (int)key.at(i).toLatin1();
            if (tmp < 0)
            {
                tmp += 256;
            }
            str.setNum(tmp);
            out.write(" ");
            out.write(str.toStdString().c_str());
        }
        out.write("\n");
    }
    out.close();
}

/**
 * @return (0;1), most values will be near 0
 */
float expRand()
{
    float x = qrand();
    x /= INT_MAX;
    float ret = x*x*x;
    if (ret < 0)
    {
        ret = 0;
    }
    if (ret > 1)
    {
        ret = 1;
    }
    return ret;
}

void Statistic::setUsedWords(QList<QString>* words)
{
    this->sorted->clear();
    QList<QString> keys = this->stats->keys();
    for (int i=0; i<words->size(); i++)
    {
        QString actualWord = words->at(i);
        float actualBalance = 0;
        for (int j=0; j<keys.size(); j++)
        {
            if (actualWord.contains(keys.at(j)))
            {
                float successes = this->stats->value(keys.at(j)).first;
                float mistakes = this->stats->value(keys.at(j)).second;
                float balance = mistakes / (mistakes + successes);
                actualBalance += balance;
            }
        }
        this->sorted->append(qMakePair(actualWord, actualBalance));
    }
    for (int i=0; i<this->sorted->size(); i++)
    {
        for (int j=0; j<this->sorted->size()-i-1; j++)
        {
            if (this->sorted->at(j).second < this->sorted->at(j+1).second)
            {
                QPair<QString, float> p = this->sorted->at(j);
                this->sorted->replace(j, this->sorted->at(j+1));
                this->sorted->replace(j+1, p);
            }
        }
    }
}

const QString& Statistic::getRecommendedWord()
{
    int index = expRand()*this->sorted->size()-1;
    if (index < 0)
    {
        index = 0;
    }
    return this->sorted->at(index).first;
}

void Statistic::reportSuccess(const QChar& prev, const QChar& actual, const QChar& next)
{
    QPair<unsigned int, unsigned int> p1 = this->stats->value(actual);
    p1.first++;
    this->stats->insert(actual, p1);
    if (prev != QChar('\0'))
    {
        QPair<unsigned int, unsigned int> p2 = this->stats->value(QString(prev)+QString(actual));
        p2.first++;
        this->stats->insert(QString(prev)+QString(actual), p2);
    }
    if (next != QChar('\0'))
    {
        QPair<unsigned int, unsigned int> p2 = this->stats->value(QString(actual)+QString(next));
        p2.first++;
        this->stats->insert(QString(actual)+QString(next), p2);
    }
}

void Statistic::reportMistake(const QChar& prev, const QChar& actual, const QChar& next)
{
    QPair<unsigned int, unsigned int> p1 = this->stats->value(actual);
    p1.second++;
    this->stats->insert(actual, p1);
    if (prev != QChar('\0'))
    {
        QPair<unsigned int, unsigned int> p2 = this->stats->value(QString(prev)+QString(actual));
        p2.second++;
        this->stats->insert(QString(prev)+QString(actual), p2);
    }
    if (next != QChar('\0'))
    {
        QPair<unsigned int, unsigned int> p2 = this->stats->value(QString(actual)+QString(next));
        p2.second++;
        this->stats->insert(QString(actual)+QString(next), p2);
    }
}
