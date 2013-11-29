#include "inputfield.h"
#include <cstdio>
#include <QKeyEvent>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QAction>
#include <QScrollBar>

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
    delete this->timeoutTimer;
    delete this->sessionTimer;
    delete this->guiUpdateTimer;
    delete this->elapsed;
}

InputField::InputField(QObject *parent) :
    QTextEdit((QWidget*)parent),
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
        this->settings->setValue("fontSize", 20);
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
        this->settings->setValue("influencingSessions", 5);
    }
    if (!this->settings->contains("sessionDuration"))
    {
        this->settings->setValue("sessionDuration", 300);
    }
    if (!this->settings->contains("timeout"))
    {
        this->settings->setValue("timeout", 3000);
    }
    qDebug() << "generated missing settings";
    this->fontSize = this->settings->value("fontSize").toInt();
    this->sound = new QSound(QString("sounds")+QDir::separator()+QString("err.wav"));
    this->timeoutTimer = new QTimer();
    this->guiUpdateTimer = new QTimer();
    this->sessionTimer = new QTimer();
    this->elapsed = new QElapsedTimer();
    connect(this->timeoutTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    connect(this->guiUpdateTimer, SIGNAL(timeout()), this, SLOT(handleGuiUpdate()));
    connect(this->sessionTimer, SIGNAL(timeout()), this, SLOT(handleSessionEnd()));
    connect(this, SIGNAL(correctTextTyped()), this, SLOT(scrollDisplay()));
    //connect(this, SIGNAL(scrollDisplay2(int)), this->display->horizontalScrollBar(), SLOT(setValue(int)));
}

void InputField::scrollDisplay()
{
    emit scrollDisplay2((int)(((float)this->correctTextLength/this->display->toPlainText().split("\n").at(0).length())*this->display->horizontalScrollBar()->maximum()));
}

void InputField::handleTimeout()
{
    this->timeout = true;
    this->sessionTimer->stop();
    this->stats->timeout();
    this->timeUntilEnd -= this->elapsed->elapsed();
    this->elapsed->restart();
}

void InputField::handleGuiUpdate()
{
    if (!this->timeout)
    {
        this->timeUntilEnd -= this->elapsed->elapsed();
        this->elapsed->restart();
    }
    this->timeLeftLabel->setText(QString::number(this->timeUntilEnd/1000, 10)+" Sekunden");
    this->hitsLabel->setText(QString::number(corrects, 10)+" Richtige");
    this->mistakesLabel->setText(QString::number(mistakes, 10)+" Fehler");
    if (this->sessionTimer->interval()-this->timeUntilEnd == 0)
    {
        this->hitsPerMinuteLabel->setText("0 Anschläge/Minute");
    }
    else
    {
        this->hitsPerMinuteLabel->setText(QString::number((int)(corrects/((float)(this->sessionTimer->interval()-this->timeUntilEnd)/60)), 10)+" Anschläge/Minute");
    }
    if (corrects == 0)
    {
        this->mistakesRateLabel->setText("0 % Fehlerrate");
    }
    else
    {
        this->mistakesRateLabel->setText(QString::number((double)mistakes*100/corrects)+" % Fehlerrate");
    }
    this->guiUpdateTimer->setInterval(1000);
    this->guiUpdateTimer->setSingleShot(true);
    this->guiUpdateTimer->start();
}

void InputField::handleSessionEnd()
{
    int statsCounter = this->settings->value("statsCounter").toInt();
    this->stats->save(statsCounter, corrects, mistakes, this->lesson);
    statsCounter++;
    this->settings->setValue("statsCounter", QVariant(statsCounter));
    setReadOnly(true);
    clear();
    display->clear();
    this->timeoutTimer->stop();
    this->guiUpdateTimer->stop();
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

void InputField::preinit(QTextEdit* display, Statistic* stats, QMainWindow* mw, QLabel* keyboard, QLabel* timeLeftLabel, QLabel* hitsLabel, QLabel* mistakesLabel, QLabel* hitsPerMinuteLabel, QLabel* mistakesRateLabel)
{
    this->display = display;
    this->stats = stats;
    this->mw = mw;
    this->keyboard = keyboard;
    this->keyboard->setPixmap(neo1);
    this->timeLeftLabel = timeLeftLabel;
    this->hitsLabel = hitsLabel;
    this->mistakesLabel = mistakesLabel;
    this->hitsPerMinuteLabel = hitsPerMinuteLabel;
    this->mistakesRateLabel = mistakesRateLabel;
    setReadOnly(true);
}

void InputField::init()
{
    qDebug() << "starting session";
    mistakes = 0;
    corrects = 0;
    errorstate = false;
    this->stats->load();
    QAction* action = (QAction*)QObject::sender();
    this->lesson = action->iconText();
    this->words = loadWordpool(this->lesson);
    display->clear();
    clear();
    setReadOnly(false);
    showText();
    this->timeout = false;
    this->firstKeyPress = true;
    this->correctTextLength = 0;
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
    if (this->firstKeyPress)
    {
        this->timeoutTimer->setInterval(this->settings->value("timeout").toInt());
        this->timeoutTimer->setSingleShot(true);
        this->timeoutTimer->start();
        this->guiUpdateTimer->setInterval(1000);
        this->guiUpdateTimer->setSingleShot(true);
        this->guiUpdateTimer->start();
        this->sessionTimer->setInterval(this->settings->value("sessionDuration").toInt()*1000);
        this->sessionTimer->setSingleShot(true);
        this->sessionTimer->start();
        this->firstKeyPress = false;
        this->timeUntilEnd = this->settings->value("sessionDuration").toInt()*1000;
        this->elapsed->start();
        return;
    }
    if (this->timeout)
    {
        this->sessionTimer->start(this->timeUntilEnd);
        this->elapsed->restart();
        qDebug() << this->timeUntilEnd;
    }
    else
    {
        this->timeoutTimer->stop();
    }
    this->timeoutTimer->setInterval(this->settings->value("timeout").toInt());
    this->timeoutTimer->setSingleShot(true);
    this->timeoutTimer->start();
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
        QString key = e->text();
        if (key != QString(""))
        {
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
                int prevprevIndex = typedText.length()-2-1;
                int prevIndex = typedText.length()-1-1;
                int nextnextIndex = typedText.length()+2-1;
                int nextIndex = typedText.length()+1-1;
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
                if (!this->timeout)
                {
                    stats->reportSuccess(prevprev, prev, displayText.at(typedText.length()-1), next, nextnext);
                    this->correctTextLength = typedText.length();
                    emit correctTextTyped();
                }
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
                format3.setBackground(QBrush(QColor("lightgrey")));
                format3.setForeground(QBrush(QColor("black")));
                format3.setFontWeight(QFont::Normal);
                format3.setFontPointSize(this->fontSize);
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
                    int prevprevIndex = typedText.length()-2-1;
                    int prevIndex = typedText.length()-1-1;
                    int nextnextIndex = typedText.length()+2-1;
                    int nextIndex = typedText.length()+1-1;
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
                    if (!this->timeout)
                    {
                        stats->reportMistake(prevprev, prev, displayText.at(typedText.length()-1), next, nextnext);
                    }
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
    this->timeout = false;
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
        format.setFontWeight(QFont::Normal);
        format.setFontPointSize(this->fontSize);
        format.setBackground(QBrush(QColor("lightgrey")));
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
        if (!word.contains(" "))
        {
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
            cursor2.insertText("\n");
            cursor2.insertText(this->words->getRandomWord());
            cursor2.insertText("\n");
        }
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
        format.setFontWeight(QFont::Normal);
        format.setFontPointSize(this->fontSize);
        format.setBackground(QBrush(QColor("lightgrey")));
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
        QString word = words->getRandomWord();
        cursor2.insertText(word);
        if (!word.contains(" "))
        {
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
        }
        else
        {
            cursor2.insertText("\n");
        }
        clear();
    }
}
