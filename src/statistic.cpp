#include "statistic.h"
#include <QTime>
#include <cstdio>
#include <QMap>
#include <climits>
#include <QFile>
#include <QList>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QDate>
#include <cmath>

Statistic::Statistic()
{
    this->sorted = new QList<QPair<QString, float> >();
    this->stats = new QMap<QString, QPair<unsigned int, unsigned int> >();
    qsrand(QTime::currentTime().msec());
}

void Statistic::load()
{
    this->sorted->clear();
    this->stats->clear();
    int statsCounter = 0;
    QSettings settings("settings.ini", QSettings::IniFormat);
    if (settings.contains("statsCounter"))
    {
        statsCounter = settings.value("statsCounter").toInt();
    }
    for (int i=statsCounter-1; i>=0 && i+settings.value("influencingSessions").toInt()>=statsCounter; i--)
    {
        load(i);
    }
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
    unsigned int lineCounter = 0;
    while (true)
    {
        QByteArray bytes = in.readLine();
        if (lineCounter < 4)
        {
            lineCounter++;
            continue;
        }
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
        if (key.length() > 1)
        {
            if (this->stats->contains(key))
            {
                successes += this->stats->value(key).first;
                mistakes += this->stats->value(key).second;
            }
            this->stats->insert(key, qMakePair(successes, mistakes));
        }
        lineCounter++;
    }
    in.close();
}

void Statistic::save(unsigned int number, unsigned int corrects, unsigned int mistakes, QString& lesson)
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
    out.write(QString(QDate::currentDate().toString()+"\n").toLatin1());
    out.write(QString(lesson+"\n").toLatin1());
    out.write(QString(QString::number(corrects, 10)+"\n").toLatin1());
    out.write(QString(QString::number(mistakes, 10)+"\n").toLatin1());
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
#ifdef __linux__
    x /= INT_MAX;
#endif
#ifdef _WIN32
    x /= 32768-1;
#endif
    QSettings s("settings.ini", QSettings::IniFormat);
    float ret = pow(x, s.value("pow").toFloat());
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

void Statistic::reportSuccess(const QChar& prevprev, const QChar& prev, const QChar& actual, const QChar& next, const QChar& nextnext)
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
    if (prevprev != QChar('\0') && prev != QChar('\0'))
    {
        QPair<unsigned int, unsigned int> p2 = this->stats->value(QString(prevprev)+QString(prev)+QString(actual));
        p2.first++;
        this->stats->insert(QString(prevprev)+QString(prev)+QString(actual), p2);
    }
    if (next != QChar('\0') && prev != QChar('\0'))
    {
        QPair<unsigned int, unsigned int> p2 = this->stats->value(QString(prev)+QString(actual)+QString(next));
        p2.first++;
        this->stats->insert(QString(prev)+QString(actual)+QString(next), p2);
    }
    if (nextnext != QChar('\0') && next != QChar('\0'))
    {
        QPair<unsigned int, unsigned int> p2 = this->stats->value(QString(actual)+QString(next)+QString(nextnext));
        p2.first++;
        this->stats->insert(QString(actual)+QString(next)+QString(nextnext), p2);
    }
}

void Statistic::reportMistake(const QChar& prevprev, const QChar& prev, const QChar& actual, const QChar& next, const QChar& nextnext)
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
    if (prevprev != QChar('\0') && prev != QChar('\0'))
    {
        QPair<unsigned int, unsigned int> p2 = this->stats->value(QString(prevprev)+QString(prev)+QString(actual));
        p2.second++;
        this->stats->insert(QString(prevprev)+QString(prev)+QString(actual), p2);
    }
    if (next != QChar('\0') && prev != QChar('\0'))
    {
        QPair<unsigned int, unsigned int> p2 = this->stats->value(QString(prev)+QString(actual)+QString(next));
        p2.second++;
        this->stats->insert(QString(prev)+QString(actual)+QString(next), p2);
    }
    if (nextnext != QChar('\0') && next != QChar('\0'))
    {
        QPair<unsigned int, unsigned int> p2 = this->stats->value(QString(actual)+QString(next)+QString(nextnext));
        p2.second++;
        this->stats->insert(QString(actual)+QString(next)+QString(nextnext), p2);
    }
}
