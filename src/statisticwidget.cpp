#include "statisticwidget.h"
#include "ui_statisticwidget.h"
#include <QGraphicsScene>
#include <QList>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDate>
#include <QGraphicsRectItem>

int mapCharToFinger(char c);

StatisticWidget::StatisticWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatisticWidget)
{
    ui->setupUi(this);
    QList<unsigned int> corrects;
    QList<unsigned int> mistakes;
    QList<QDate> dates;
    QList<QString> lessons;
    QMap<QChar, QPair<unsigned int, unsigned int> > stats;
    QMap<unsigned int, QPair<unsigned int, unsigned int> > fingerStats;

    int statsCounter = 0;
    QSettings settings("settings.ini", QSettings::IniFormat);
    if (settings.contains("statsCounter"))
    {
        statsCounter = settings.value("statsCounter").toInt();
    }
    for (int i=0; i<statsCounter; i++)
    {
        QString path = QString("stats")+QDir::separator()+QString::number(i, 10)+QString(".stats");
        QFile in(path);
        if (!in.exists())
        {
            qDebug() << "file \"" << path << "\" does not exist";
            continue;
        }
        if (!in.open(QIODevice::ReadOnly))
        {
            qDebug() << "file \"" << path << "\" not openable";
            continue;
        }
        unsigned int lineCounter = 0;
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
            QDate date;
            switch (lineCounter)
            {
                case 0:
                    date = QDate::fromString(line);
                    dates.append(date);
                    break;
                case 1:
                    lessons.append(line);
                    break;
                case 2:
                    corrects.append(line.toInt());
                    break;
                case 3:
                    mistakes.append(line.toInt());
                    break;
                default:
                    QStringList sl = line.split(" ");
                    unsigned int successes = sl.at(0).toInt();
                    unsigned int mistakes = sl.at(1).toInt();
                    QString key;
                    for (int i=2; i<sl.size(); i++)
                    {
                        key += QChar(sl.at(i).toInt());
                    }
                    if (key.length() == 1)
                    {
                        if (stats.contains(key.at(0)))
                        {
                            successes += stats.value(key.at(0)).first;
                            mistakes += stats.value(key.at(0)).second;
                        }
                        stats.insert(key.at(0), qMakePair(successes, mistakes));
                    }
                    break;
            }
            lineCounter++;
        }
        in.close();
    }

    QSettings s("settings.ini", QSettings::IniFormat);
    QList<QChar> keys = stats.keys();
    for (int i=0; i<keys.size(); i++)
    {
        int finger = mapCharToFinger(keys.at(i).toLatin1());
        if (finger != -1)
        {
            unsigned int success = stats.value(keys.at(i)).first;
            unsigned int mistakes = stats.value(keys.at(i)).second;
            success += fingerStats.value(finger).first;
            mistakes += fingerStats.value(finger).second;
            fingerStats.insert(finger, qMakePair(success, mistakes));
        }
    }

    float minA = 10000;
    float maxA = 0;
    for (int i=0; i<corrects.size(); i++)
    {
        float a = (float)(corrects.at(i)+mistakes.at(i))/5;
        if (a < minA)
        {
            minA = a;
        }
        if (a > maxA)
        {
            maxA = a;
        }
    }
    int goodLineA = s.value("goodLineA").toInt();
    int badLineA = s.value("badLineA").toInt();
    QList<float> points;
    for (int i=0; i<corrects.size(); i++)
    {
        float a = (float)(corrects.at(i)+mistakes.at(i))/5;
        points.append(((a-minA)/(maxA-minA))*200);
    }
    QGraphicsScene* scene = new QGraphicsScene(0,0,5*corrects.size(),200);
    if (maxA > goodLineA)
    {
        QGraphicsRectItem* rect = new QGraphicsRectItem(0, 200-200, (points.size()-1)*5, 200);
        rect->setBrush(QBrush(QColor("green")));
        scene->addItem(rect);
    }
    if (maxA > badLineA && minA < goodLineA)
    {
        QGraphicsRectItem* rect = new QGraphicsRectItem(0, 200-((goodLineA-minA)/(maxA-minA))*200, (points.size()-1)*5, 200);
        rect->setBrush(QBrush(QColor("orange")));
        scene->addItem(rect);
    }
    if (minA < badLineA)
    {
        QGraphicsRectItem* rect = new QGraphicsRectItem(0, 200-((badLineA-minA)/(maxA-minA))*200, (points.size()-1)*5, 200);
        rect->setBrush(QBrush(QColor("red")));
        scene->addItem(rect);
    }
    for (int i=0; i<points.size()-1; i++)
    {
        scene->addLine(i*5, 200-points.at(i), (i+1)*5, 200-points.at(i+1));
    }
    this->ui->plot->setScene(scene);

    float goodLineE = s.value("goodLineE").toFloat();
    float badLineE = s.value("badLineE").toFloat();
    points.clear();
    QGraphicsScene* scene2 = new QGraphicsScene(0,0,5*corrects.size(),200);
    minA = 100;
    maxA = 0;
    for (int i=0; i<corrects.size(); i++)
    {
        float a = (float)mistakes.at(i)*100/(corrects.at(i)+mistakes.at(i));
        if (a < minA)
        {
            minA = a;
        }
        if (a > maxA)
        {
            maxA = a;
        }
    }
    for (int i=0; i<corrects.size(); i++)
    {
        float a = (float)mistakes.at(i)*100/(corrects.at(i)+mistakes.at(i));
        points.append(((a-minA)/(maxA-minA))*200);
    }
    if (maxA > badLineE)
    {
        QGraphicsRectItem* rect = new QGraphicsRectItem(0, 200-200, (points.size()-1)*5, 200);
        rect->setBrush(QBrush(QColor("red")));
        scene2->addItem(rect);
    }
    if (maxA > goodLineE && minA < badLineE)
    {
        QGraphicsRectItem* rect = new QGraphicsRectItem(0, 200-((badLineE-minA)/(maxA-minA))*200, (points.size()-1)*5, 200);
        rect->setBrush(QBrush(QColor("orange")));
        scene2->addItem(rect);
    }
    if (minA < goodLineE)
    {
        QGraphicsRectItem* rect = new QGraphicsRectItem(0, 200-((goodLineE-minA)/(maxA-minA))*200, (points.size()-1)*5, 200);
        rect->setBrush(QBrush(QColor("green")));
        scene2->addItem(rect);
    }
    for (int i=0; i<points.size()-1; i++)
    {
        scene2->addLine(i*5, 200-points.at(i), (i+1)*5, 200-points.at(i+1));
    }
    this->ui->plot2->setScene(scene2);

    for (int i=0; i<8; i++)
    {
        if (fingerStats.contains(i))
        {
            float f = (float)fingerStats.value(i).second*100 / (fingerStats.value(i).second+fingerStats.value(i).first);
            char tmp[16];
            sprintf(tmp, "%.1f %%", f);
            QString str = QString(tmp);
            switch (i)
            {
                case 0:
                    this->ui->finger0->setText(str);
                    break;
                case 1:
                    this->ui->finger1->setText(str);
                    break;
                case 2:
                    this->ui->finger2->setText(str);
                    break;
                case 3:
                    this->ui->finger3->setText(str);
                    break;
                case 4:
                    this->ui->finger4->setText(str);
                    break;
                case 5:
                    this->ui->finger5->setText(str);
                    break;
                case 6:
                    this->ui->finger6->setText(str);
                    break;
                case 7:
                    this->ui->finger7->setText(str);
                    break;
            }
        }
        else
        {

            switch (i)
            {
                case 0:
                    this->ui->finger0->setText("0.0 %");
                    break;
                case 1:
                    this->ui->finger1->setText("0.0 %");
                    break;
                case 2:
                    this->ui->finger2->setText("0.0 %");
                    break;
                case 3:
                    this->ui->finger3->setText("0.0 %");
                    break;
                case 4:
                    this->ui->finger4->setText("0.0 %");
                    break;
                case 5:
                    this->ui->finger5->setText("0.0 %");
                    break;
                case 6:
                    this->ui->finger6->setText("0.0 %");
                    break;
                case 7:
                    this->ui->finger7->setText("0.0 %");
                    break;
            }
        }
    }
}

int mapCharToFinger(char c)
{
    switch (c)
    {
        case 'x':
        case 'u':
        case 'ü':
        case '1':
        case '2':
        case '°':
        case '§':
        case 'X':
        case 'U':
        case 'Ü':
            return 0;
        case '3':
        case 'v':
        case 'i':
        case 'ö':
        case 'ℓ':
        case 'V':
        case 'I':
        case 'Ö':
            return 1;
        case '4':
        case 'l':
        case 'a':
        case 'ä':
        case '»':
        case 'L':
        case 'A':
        case 'Ä':
            return 2;
        case '5':
        case '6':
        case 'c':
        case 'e':
        case 'p':
        case 'w':
        case 'o':
        case 'z':
        case '«':
        case 'C':
        case 'E':
        case 'P':
        case '$':
        case 'W':
        case 'O':
        case 'Z':
            return 3;
        case '7':
        case '8':
        case 'k':
        case 'h':
        case 's':
        case 'n':
        case 'b':
        case 'm':
        case '„':
        case 'K':
        case 'S':
        case 'B':
        case '€':
        case 'H':
        case 'N':
        case 'M':
            return 4;
        case '9':
        case 'g':
        case 'r':
        case ',':
        case '“':
        case 'G':
        case 'R':
        case '–':
            return 5;
        case '0':
        case 'f':
        case 't':
        case '.':
        case '”':
        case 'F':
        case 'T':
        case '•':
            return 6;
        case '-':
        case 'q':
        case 'ß':
        case 'd':
        case 'y':
        case 'j':
        case 'D':
        case 'J':
        case 'Y':
        case 'Q':
        case '—':
            return 7;
        default:
            return -1;
    }
}