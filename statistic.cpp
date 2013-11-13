#include "statistic.h"
#include <QTime>
#include <cstdio>
#include <QMap>
#include <climits>
#include <QFile>
#include <QList>

Statistic::Statistic():bla(42)
{
    this->sorted = new QList<QPair<QString, float> >();
    this->stats = new QMap<QString, QPair<unsigned int, unsigned int> >();
    qsrand(QTime::currentTime().msec());
    load(0);
    sort();
}

void Statistic::sort()
{
    QList<QString> keys = this->stats->keys();
    for (int i=0; i<keys.size(); i++)
    {
        this->sorted->append(qMakePair(keys.at(i), (float)(this->stats->value(keys.at(i)).second) / (float)(this->stats->value(keys.at(i)).second) + (float)(this->stats->value(keys.at(i)).first)));
    }
    for (int i=0; i<this->sorted->size(); i++)
    {
        for (int j=0; i<this->sorted->size()-i-1; j++)
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

void Statistic::load(unsigned int number)
{
    //TODO load from hdd
}

void Statistic::save(unsigned int number)
{
    QString path = "C:\\Users\\bla\\Desktop\\neo\\stats\\"+QString::number(number, 10)+".stats";
	QFile out(path);
	out.open(QIODevice::WriteOnly);
    QList<QString> keys = this->stats->keys();
    for (int i=0; i<keys.size(); i++)
    {
		QString key = keys.at(i);
        unsigned int successes = this->stats->value(keys.at(i)).first;
        unsigned int mistakes = this->stats->value(keys.at(i)).second;
        for (int i=0; i<key.length(); i++)
		{
            QString str;
            int tmp = (int)key.at(i).toLatin1();
            if (tmp < 0)
            {
                tmp += 256;
            }
            str.setNum(tmp);
            out.write(str.toStdString().c_str());
            out.write(" ");
            str.setNum(successes);
            out.write(str.toStdString().c_str());
            out.write(" ");
            str.setNum(mistakes);
            out.write(str.toStdString().c_str());
            out.write("\n");
		}
    }
    out.close();
}

/**
 * @return (0;1), most values will be near 0
 */
float expRand()
{
    float x = qrand()/INT_MAX;
    float ret = x*x;
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

const QString& Statistic::getRecommendedWord(QList<QString>& words)
{
    if (this->sorted->size() == 0)
    {
        return words.at(qrand()%(words.size()));
    }
    int index = expRand()*this->sorted->size()-1;
    if (index < 0)
    {
        index = 0;
    }
    QString attribute = this->sorted->at(index).first;
    QList<QString> possible;
    for (int i=0; i<this->sorted->size(); i++)
    {
        if (this->sorted->at(i).first.contains(attribute))
        {
            possible.append(this->sorted->at(i).first);
        }
    }
    index = qrand()%possible.size();
    return possible.at(index);
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
