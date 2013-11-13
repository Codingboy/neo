#include "inputfield.h"
#include <cstdio>
#include <QKeyEvent>
#include <QFile>
#include <QDebug>

InputField::InputField(QObject *parent) :
    QTextEdit((QWidget*)parent),
    startTime(QTime::currentTime()),
    neo1("./graphics/neo1.png"),
    neo2("./graphics/neo2.png"),
    neo3("./graphics/neo3.png")
{
        display = NULL;
        corrects = 0;
        hits = 0;
        mistakes = 0;
        errorstate = false;
}

Wordpool* InputField::loadWordpool(int lektion)
{
    QString path = "C:\\Users\\bla\\Desktop\\neo\\wp\\"+QString::number(lektion, 10)+".wp";
    QFile in(path);
    if (!in.open(QIODevice::ReadOnly))
    {
        qDebug() << "file not openable";
    }
    unsigned int linecounter = 0;
    Wordpool* wp = new Wordpool(this->stats);
    while (true)
    {
        QByteArray bytes = in.readLine();
        if (bytes.size() == 0)
        {
            break;
        }
        QString str(bytes);
        if (linecounter == 0)
        {

        }
        else
        {
            wp->addWord(str);
        }
        linecounter++;
    }
    in.close();
    return wp;
}

void InputField::init(QTextEdit* display, int lektion, Statistic* stats, unsigned int time, QMainWindow* mw, QLabel* keyboard)
{
    this->display = display;
    this->stats = stats;
    this->words = loadWordpool(lektion);
    this->time = time;
    this->mw = mw;
    this->keyboard = keyboard;
    showText();
}

void InputField::keyReleaseEvent(QKeyEvent *e)
{
    if (e->modifiers() & Qt::ShiftModifier)
    {
        keyboard->setPixmap(neo2);
    }
    else
    {
        keyboard->setPixmap(neo1);
    }
    QTextEdit::keyPressEvent(e);
}

void InputField::keyPressEvent(QKeyEvent* e)
{
    if (e->modifiers() & Qt::ShiftModifier)
    {
        keyboard->setPixmap(neo2);
    }
    else
    {
        keyboard->setPixmap(neo1);
    }
    if (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete || e->key() == Qt::Key_Left || e->key() == Qt::Key_Right)
    {
        QTextEdit::keyPressEvent(e);
    }
    else
    {
        QString key = e->text();
        if (key != QString(""))
        {
            hits++;
            if (e->key() == Qt::Key_Return)
            {
                key = QString("\n");
            }
            QString typedText = toPlainText()+key;
            QString displayText = this->display->toPlainText();
            if (displayText.left(typedText.length()).compare(typedText) == 0)
            {
                corrects++;
                errorstate = false;
                stats->reportSuccess(displayText.at(typedText.length()-1), displayText.at(typedText.length()), displayText.at(typedText.length()+1));
                QTextCursor cursor(textCursor());
                QTextCharFormat format;
                format.setBackground(QBrush(QColor("white")));
                format.setForeground(QBrush(QColor("green")));
                cursor.setCharFormat(format);
                cursor.insertText(key);
                if (e->key() == Qt::Key_Return)
                {
                    showText();
                }
            }
            else
            {
                if (!errorstate)//protect from followed mistakes
                {
                    mistakes++;
                    errorstate = true;
                    stats->reportMistake(displayText.at(typedText.length()-1), displayText.at(typedText.length()), displayText.at(typedText.length()+1));
                }
                QTextCursor cursor(textCursor());
                QTextCharFormat format;
                format.setBackground(QBrush(QColor("red")));
                format.setForeground(QBrush(QColor("black")));
                cursor.setCharFormat(format);
                cursor.insertText(key);
            }
            printf("%u %u %c\n", corrects, mistakes, (char)e->key());
        }
    }
    QTime t = this->startTime.addSecs(this->time);
    if (t <= QTime::currentTime())
    {
        this->stats->save(0);
        //TODO show evaluation
        setReadOnly(true);
        //mw->close();
    }
    //printf("%s\n", toStdString().c_str());
}

void InputField::showText()
{
    if (this->display->toPlainText().length() == 0)
    {
        qDebug() << this->words->getRandomWord();
        this->display->insertPlainText(this->words->getRandomWord());
        this->display->insertPlainText(" ");
        this->display->insertPlainText(this->words->getRandomWord());
        this->display->insertPlainText(" ");
        this->display->insertPlainText(this->words->getRandomWord());
        this->display->insertPlainText(" ");
        this->display->insertPlainText(this->words->getRandomWord());
        this->display->insertPlainText("\n");
        this->display->insertPlainText(this->words->getRandomWord());
        this->display->insertPlainText(" ");
        this->display->insertPlainText(this->words->getRandomWord());
        this->display->insertPlainText(" ");
        this->display->insertPlainText(this->words->getRandomWord());
        this->display->insertPlainText(" ");
        this->display->insertPlainText(this->words->getRandomWord());
        this->display->insertPlainText("\n");
    }
    else
    {
        QString content = this->display->toPlainText();
        int counter = 0;
        int n = 1;
        for (int i=content.length()-1; i>=0; i--)
        {
            if (content.at(i) == '\n')
            {
                counter++;
                if (counter == 2)
                {
                    break;
                }
            }
            else
            {
                n++;
            }
        }
        QString last = content.right(n);
        this->display->clear();
        this->display->insertPlainText(last);
        this->display->insertPlainText(words->getRandomWord());
        this->display->insertPlainText(" ");
        this->display->insertPlainText(words->getRandomWord());
        this->display->insertPlainText(" ");
        this->display->insertPlainText(words->getRandomWord());
        this->display->insertPlainText(" ");
        this->display->insertPlainText(words->getRandomWord());
        this->display->insertPlainText("\n");
        clear();
    }
}
