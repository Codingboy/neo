#include "inputfield.h"
#include <cstdio>
#include <QKeyEvent>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QAction>

InputField::InputField(QObject *parent) :
    QTextEdit((QWidget*)parent),
    startTime(QTime::currentTime()),
    neo1(QString("graphics")+QDir::separator()+QString("neo1.png")),
    neo2(QString("graphics")+QDir::separator()+QString("neo2.png")),
    neo3(QString("graphics")+QDir::separator()+QString("neo3.png"))
{
        display = NULL;
        corrects = 0;
        hits = 0;
        mistakes = 0;
        errorstate = false;
}

Wordpool* InputField::loadWordpool(QString& lesson)
{
    QString path = QString("wp")+QDir::separator()+lesson;
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
        str.remove("\n");
        str.remove("\r");
        if (linecounter == 0)
        {

        }
        else
        {
            wp->addWord(str);
        }
        linecounter++;
    }
    wp->finalise();
    in.close();
    return wp;
}

void InputField::preinit(QTextEdit* display, Statistic* stats, unsigned int time, QMainWindow* mw, QLabel* keyboard)
{
    this->display = display;
    this->stats = stats;
    this->time = time;
    this->mw = mw;
    this->keyboard = keyboard;
    setReadOnly(true);
}

void InputField::init()
{
    mistakes = 0;
    corrects = 0;
    hits = 0;
    errorstate = false;
    QAction* action = (QAction*)QObject::sender();
    QString lesson = action->iconText();
    this->words = loadWordpool(lesson);
    this->startTime = QTime::currentTime();
    display->clear();
    clear();
    setReadOnly(false);
    showText();
}

void InputField::keyReleaseEvent(QKeyEvent *e)
{
    if (isReadOnly())
    {
        return;
    }
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
    if (isReadOnly())
    {
        return;
    }
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
                format.setFontPointSize(14);
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
                format.setFontPointSize(14);
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
        clear();
        display->clear();
        QString text;
        text += "hits/minute:\t";
        text += QString::number((float)(corrects+mistakes)/((float)(this->time)/60))+"\n";
        text += "mistakes/100hits:\t";
        text += QString::number((float)mistakes*100/(corrects+mistakes))+"\n";
        display->setText(text);
    }
    //printf("%s\n", toStdString().c_str());
}

void InputField::showText()
{
    if (this->display->toPlainText().length() == 0)
    {
        QTextCursor cursor(this->display->textCursor());
        QTextCharFormat format;
        format.setFontPointSize(14);
        cursor.setCharFormat(format);
        cursor.insertText(this->words->getRandomWord());
        cursor.insertText(" ");
        cursor.insertText(this->words->getRandomWord());
        cursor.insertText(" ");
        cursor.insertText(this->words->getRandomWord());
        cursor.insertText(" ");
        cursor.insertText(this->words->getRandomWord());
        cursor.insertText("\n");
        cursor.insertText(this->words->getRandomWord());
        cursor.insertText(" ");
        cursor.insertText(this->words->getRandomWord());
        cursor.insertText(" ");
        cursor.insertText(this->words->getRandomWord());
        cursor.insertText(" ");
        cursor.insertText(this->words->getRandomWord());
        cursor.insertText("\n");
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
        QTextCursor cursor(this->display->textCursor());
        QTextCharFormat format;
        format.setFontPointSize(14);
        cursor.setCharFormat(format);
        cursor.insertText(last);
        cursor.insertText(words->getRandomWord());
        cursor.insertText(" ");
        cursor.insertText(words->getRandomWord());
        cursor.insertText(" ");
        cursor.insertText(words->getRandomWord());
        cursor.insertText(" ");
        cursor.insertText(words->getRandomWord());
        cursor.insertText("\n");
        clear();
    }
}
