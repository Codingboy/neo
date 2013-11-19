#include "inputfield.h"
#include <cstdio>
#include <QKeyEvent>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QAction>

InputField::~InputField()
{
#ifdef _WIN32
    this->neo20->kill();
    delete this->neo20;
    qDebug() << "qwertz keylayout active";
#endif
#ifdef __linux__
    QProcess setxkbmap;
    QStringList args;
    args << "-c" << "setxkbmap de && xset r 51";
    setxkbmap.start(QString("/bin/bash"), args);
    setxkbmap.waitForFinished();
    qDebug() << "qwertz keylayout active";
#endif
    delete this->settings;
    delete this->sound;
    if (this->words == NULL)
    {
        delete this->words;
    }
}

InputField::InputField(QObject *parent) :
    QTextEdit((QWidget*)parent),
    startTime(QTime::currentTime()),
    neo1(QString("graphics")+QDir::separator()+QString("neo1.png")),
    neo2(QString("graphics")+QDir::separator()+QString("neo2.png")),
    neo3(QString("graphics")+QDir::separator()+QString("neo3.png")),
    words(NULL)
{
#ifdef _WIN32
    this->neo20 = new QProcess();
    this->neo20->start(QString("external")+QDir::separator()+QString("neo20.exe"));
    qDebug() << "neo keylayout active";
#endif
#ifdef __linux__
    QProcess setxkbmap;
    QStringList args;
    args << "-c" << "setxkbmap lv && xmodmap external/neo_de.xmodmap && xset -r 51";
    setxkbmap.start(QString("/bin/bash"), args);
    setxkbmap.waitForFinished();
    qDebug() << "neo keylayout active";
#endif
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
    if (!this->settings->contains("fontSize"))
    {
        this->settings->setValue("fontSize", 12);
    }
    if (!this->settings->contains("fontBoldSize"))
    {
        this->settings->setValue("fontBoldSize", 20);
    }
    if (!this->settings->contains("goodLineA"))
    {
        this->settings->setValue("goodLineA", 120);
    }
    if (!this->settings->contains("goodLineE"))
    {
        this->settings->setValue("goodLineE", 2);
    }
    if (!this->settings->contains("badLineA"))
    {
        this->settings->setValue("badLineA", 100);
    }
    if (!this->settings->contains("badLineE"))
    {
        this->settings->setValue("badLineE", 3);
    }
    if (!this->settings->contains("pow"))
    {
        this->settings->setValue("pow", 2.0);
    }
    if (!this->settings->contains("visualErrorFeedback"))
    {
        this->settings->setValue("visualErrorFeedback", true);
    }
    if (!this->settings->contains("influencingSessions"))
    {
        this->settings->setValue("influencingSessions", 10);
    }
    qDebug() << "settings loaded or generated";
    this->fontSize = this->settings->value("fontSize").toInt();
    this->fontBoldSize = this->settings->value("fontBoldSize").toInt();
    sound = new QSound(QString("sounds")+QDir::separator()+QString("err.wav"));
}

Wordpool* InputField::loadWordpool(QString& lesson)
{
    QString path = QString("wp")+QDir::separator()+lesson;
    QFile in(path);
    if (!in.open(QIODevice::ReadOnly))
    {
        qDebug() << "file" << path << "not openable";
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
    qDebug() << "loaded wordpool" << path;
    return wp;
}

void InputField::preinit(QTextEdit* display, Statistic* stats, unsigned int time, QMainWindow* mw, QLabel* keyboard)
{
    this->display = display;
    this->stats = stats;
    this->time = time;
    this->mw = mw;
    this->keyboard = keyboard;
    this->keyboard->setPixmap(neo1);
    setReadOnly(true);
}

void InputField::init()
{
    qDebug() << "starting session";
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
    if (e->key() == Qt::Key_Shift)
    {
        keyboard->setPixmap(neo1);
    }
    if (e->key() == -1)
    {
        keyboard->setPixmap(neo1);
    }
    if (isReadOnly())
    {
        return;
    }
    QTextEdit::keyReleaseEvent(e);
    if (this->settings->value("visualErrorFeedback").toBool())
    {
        setStyleSheet("QTextEdit { background-color: white }");
    }
}

void InputField::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Shift)
    {
        keyboard->setPixmap(neo2);
    }
    if (e->key() == Qt::Key_Mode_switch)
    {
        keyboard->setPixmap(neo3);
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
        this->fontSize = this->settings->value("fontSize").toInt();
        this->fontBoldSize = this->settings->value("fontBoldSize").toInt();
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
                int prevprevIndex = typedText.length()-2;
                int prevIndex = typedText.length()-1;
                int nextnextIndex = typedText.length()+2;
                int nextIndex = typedText.length()+1;
                QChar prevprev('\0');
                QChar prev('\0');
                QChar next('\0');
                QChar nextnext('\0');
                if (prevprevIndex >= 0)
                {
                    prevprev = displayText.at(prevprevIndex);
                }
                if (prevIndex >= 0)
                {
                    prev = displayText.at(prevIndex);
                }
                if (nextnextIndex < displayText.length())
                {
                    nextnext = displayText.at(nextnextIndex);
                }
                if (nextIndex < displayText.length())
                {
                    next = displayText.at(nextIndex);
                }
                stats->reportSuccess(prevprev, prev, displayText.at(typedText.length()), next, nextnext);
                QTextCursor cursor(textCursor());
                QTextCharFormat format;
                format.setBackground(QBrush(QColor("white")));
                format.setForeground(QBrush(QColor("green")));
                format.setFontPointSize(this->fontSize);
                cursor.setCharFormat(format);
                cursor.insertText(key);
                this->display->clear();
                QTextCursor cursor2(this->display->textCursor());
                QTextCharFormat format2;
                format2.setBackground(QBrush(QColor("white")));
                format2.setForeground(QBrush(QColor("black")));
                format2.setFontWeight(QFont::Normal);
                format2.setFontPointSize(this->fontSize);
                cursor2.setCharFormat(format2);
                cursor2.insertText(typedText);
                QTextCursor cursor3(this->display->textCursor());
                QTextCharFormat format3;
                format3.setBackground(QBrush(QColor("white")));
                format3.setForeground(QBrush(QColor("black")));
                format3.setFontWeight(QFont::Bold);
                format3.setFontPointSize(this->fontBoldSize);
                cursor3.setCharFormat(format3);
                cursor3.insertText(displayText.left(typedText.length()+1).right(1));
                QTextCursor cursor4(this->display->textCursor());
                QTextCharFormat format4;
                format4.setBackground(QBrush(QColor("white")));
                format4.setForeground(QBrush(QColor("black")));
                format4.setFontWeight(QFont::Normal);
                format4.setFontPointSize(this->fontSize);
                cursor4.setCharFormat(format4);
                cursor4.insertText(displayText.right(displayText.length()-typedText.length()-1));
                if (e->key() == Qt::Key_Return)
                {
                    showText();
                }
            }
            else
            {
                if (this->settings->value("visualErrorFeedback").toBool())
                {
                    setStyleSheet("QTextEdit { background-color: red }");
                }
                if (this->settings->value("playErrorSound").toBool())
                {
                    this->sound->play();
                }
                if (!errorstate)//protect from followed mistakes
                {
                    mistakes++;
                    errorstate = true;
                    int prevprevIndex = typedText.length()-2;
                    int prevIndex = typedText.length()-1;
                    int nextnextIndex = typedText.length()+2;
                    int nextIndex = typedText.length()+1;
                    QChar prevprev('\0');
                    QChar prev('\0');
                    QChar next('\0');
                    QChar nextnext('\0');
                    if (prevprevIndex >= 0)
                    {
                        prevprev = displayText.at(prevprevIndex);
                    }
                    if (prevIndex >= 0)
                    {
                        prev = displayText.at(prevIndex);
                    }
                    if (nextnextIndex < displayText.length())
                    {
                        nextnext = displayText.at(nextnextIndex);
                    }
                    if (nextIndex < displayText.length())
                    {
                        next = displayText.at(nextIndex);
                    }
                    stats->reportMistake(prevprev, prev, displayText.at(typedText.length()), next, nextnext);
                }
                if (!this->settings->value("blockOnError").toBool())
                {
                    QTextCursor cursor(textCursor());
                    QTextCharFormat format;
                    format.setBackground(QBrush(QColor("red")));
                    format.setForeground(QBrush(QColor("black")));
                    format.setFontPointSize(this->fontSize);
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

void InputField::abort()
{
    setReadOnly(true);
    clear();
    this->display->clear();
    qDebug() << "session aborted";
}

void InputField::showText()
{
    if (this->display->toPlainText().length() == 0)
    {
        QTextCursor cursor(this->display->textCursor());
        QTextCharFormat format;
        format.setFontWeight(QFont::Bold);
        format.setFontPointSize(this->fontBoldSize);
        format.setBackground(QBrush(QColor("white")));
        format.setForeground(QBrush(QColor("black")));
        cursor.setCharFormat(format);
        QString word = this->words->getRandomWord();
        cursor.insertText(word.left(1));

        QTextCursor cursor2(this->display->textCursor());
        QTextCharFormat format2;
        format2.setFontWeight(QFont::Normal);
        format2.setFontPointSize(this->fontSize);
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
        format.setFontPointSize(this->fontBoldSize);
        format.setBackground(QBrush(QColor("white")));
        format.setForeground(QBrush(QColor("black")));
        cursor.setCharFormat(format);
        cursor.insertText(last.left(1));
        QTextCursor cursor2(this->display->textCursor());
        QTextCharFormat format2;
        format2.setFontWeight(QFont::Normal);
        format2.setFontPointSize(this->fontSize);
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
        cursor2.insertText(" ");
        cursor2.insertText(words->getRandomWord());
        cursor2.insertText(" ");
        cursor2.insertText(words->getRandomWord());
        cursor2.insertText("\n");
        clear();
    }
}
