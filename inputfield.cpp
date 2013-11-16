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
    this->settings = new QSettings("settings.ini", QSettings::IniFormat);
    if (!this->settings->contains("statsCounter"))
    {
        this->settings->setValue("statsCounter", 0);
    }
    if (!this->settings->contains("blockOnError"))
    {
        this->settings->setValue("blockOnError", true);
    }
    if (!this->settings->contains("playErrorSound"))
    {
        this->settings->setValue("playErrorSound", true);
    }
    sound = new QSound(QString("sounds")+QDir::separator()+QString("err.wav"));
}

Wordpool* InputField::loadWordpool(QString& lesson)
{
    QString path = QString("wp")+QDir::separator()+lesson;
    QFile in(path);
    if (!in.open(QIODevice::ReadOnly))
    {
        qDebug() << "file not openable";
    }
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
        wp->addWord(str);
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
    this->stats->load();
    QAction* action = (QAction*)QObject::sender();
    this->lesson = action->iconText();
    this->words = loadWordpool(this->lesson);
    this->startTime = QTime::currentTime();
    display->clear();
    clear();
    setReadOnly(false);
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
    if (isReadOnly())
    {
        return;
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
    if (isReadOnly())
    {
        return;
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
                this->display->clear();
                QTextCursor cursor2(this->display->textCursor());
                QTextCharFormat format2;
                format2.setBackground(QBrush(QColor("white")));
                format2.setForeground(QBrush(QColor("black")));
                format2.setFontWeight(QFont::Normal);
                format2.setFontPointSize(14);
                cursor2.setCharFormat(format2);
                cursor2.insertText(typedText);
                QTextCursor cursor3(this->display->textCursor());
                QTextCharFormat format3;
                format3.setBackground(QBrush(QColor("white")));
                format3.setForeground(QBrush(QColor("black")));
                format3.setFontWeight(QFont::Bold);
                format3.setFontPointSize(20);
                cursor3.setCharFormat(format3);
                cursor3.insertText(displayText.left(typedText.length()+1).right(1));
                QTextCursor cursor4(this->display->textCursor());
                QTextCharFormat format4;
                format4.setBackground(QBrush(QColor("white")));
                format4.setForeground(QBrush(QColor("black")));
                format4.setFontWeight(QFont::Normal);
                format4.setFontPointSize(14);
                cursor4.setCharFormat(format4);
                cursor4.insertText(displayText.right(displayText.length()-typedText.length()-1));
                if (e->key() == Qt::Key_Return)
                {
                    showText();
                }
            }
            else
            {
                if (this->settings->value("playErrorSound").toBool())
                {
                    this->sound->play();
                }
                if (!this->settings->value("blockOnError").toBool())
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
            }
        }
    }
    QTime t = this->startTime.addSecs(this->time);
    if (t <= QTime::currentTime())
    {
        int statsCounter = this->settings->value("statsCounter").toInt();
        this->stats->save(statsCounter, corrects, mistakes, this->lesson);
        statsCounter++;
        this->settings->setValue("statsCounter", QVariant(statsCounter));
        setReadOnly(true);
        clear();
        display->clear();
        QString text;
        text += QString::number((float)(corrects+mistakes)/((float)(this->time)/60))+" ";
        text += "hits/minute\n";
        text += QString::number((float)mistakes*100/(corrects+mistakes))+" ";
        text += "mistakes/100hits\n";
        display->setText(text);
    }
}

void InputField::showText()
{
    if (this->display->toPlainText().length() == 0)
    {
        QTextCursor cursor(this->display->textCursor());
        QTextCharFormat format;
        format.setFontWeight(QFont::Bold);
        format.setFontPointSize(20);
        format.setBackground(QBrush(QColor("white")));
        format.setForeground(QBrush(QColor("black")));
        cursor.setCharFormat(format);
        QString word = this->words->getRandomWord();
        cursor.insertText(word.left(1));

        QTextCursor cursor2(this->display->textCursor());
        QTextCharFormat format2;
        format2.setFontWeight(QFont::Normal);
        format2.setFontPointSize(14);
        format2.setBackground(QBrush(QColor("white")));
        format2.setForeground(QBrush(QColor("black")));
        cursor2.setCharFormat(format2);
        cursor2.insertText(word.right(word.length()-1));
        cursor2.insertText(" ");
        cursor2.insertText(this->words->getRandomWord());
        cursor2.insertText(" ");
        cursor2.insertText(this->words->getRandomWord());
        cursor2.insertText(" ");
        cursor2.insertText(this->words->getRandomWord());
        cursor2.insertText("\n");
        cursor2.insertText(this->words->getRandomWord());
        cursor2.insertText(" ");
        cursor2.insertText(this->words->getRandomWord());
        cursor2.insertText(" ");
        cursor2.insertText(this->words->getRandomWord());
        cursor2.insertText(" ");
        cursor2.insertText(this->words->getRandomWord());
        cursor2.insertText("\n");
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
        format.setFontWeight(QFont::Bold);
        format.setFontPointSize(20);
        format.setBackground(QBrush(QColor("white")));
        format.setForeground(QBrush(QColor("black")));
        cursor.setCharFormat(format);
        cursor.insertText(last.left(1));
        QTextCursor cursor2(this->display->textCursor());
        QTextCharFormat format2;
        format2.setFontWeight(QFont::Normal);
        format2.setFontPointSize(14);
        format2.setBackground(QBrush(QColor("white")));
        format2.setForeground(QBrush(QColor("black")));
        cursor2.setCharFormat(format2);
        cursor2.insertText(last.right(last.length()-1));
        cursor2.insertText(words->getRandomWord());
        cursor2.insertText(" ");
        cursor2.insertText(words->getRandomWord());
        cursor2.insertText(" ");
        cursor2.insertText(words->getRandomWord());
        cursor2.insertText(" ");
        cursor2.insertText(words->getRandomWord());
        cursor2.insertText("\n");
        clear();
    }
}
