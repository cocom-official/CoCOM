#include <QScrollBar>
#include <QDebug>

#include "Console.h"

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent),
      localEchoEnabled(true),
      selfNewLineJudge(false)
{
    setPS1("->");
    setPS2(">");
    setBlockEnd("\\");

    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    setPalette(p);
    newLine();
}

Console::Console(const QString &copyright, QWidget *parent)
    : QPlainTextEdit(parent),
      localEchoEnabled(true)
{
    setPS1("->");
    setPS2(">");
    setBlockEnd("\\");

    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    setPalette(p);

    if (*copyright.end() != '\n' || *copyright.end() != '\r')
    {
        insertPlainText(copyright + QString("\n"));
    }
    else
    {
        insertPlainText(copyright);
    }
    newLine();
}

void Console::setPS1(const QString &ps1)
{
    PS1 = ps1;
    PS2 = PS2.trimmed();

    while (PS2.length() < PS1.length())
    {
        PS2.insert(0, ' ');
        if (PS2.length() > 10)
        {
            break;
        }
    }
}

void Console::setPS2(const QString &ps2)
{
    PS2 = ps2;
    while (PS2.length() < PS1.length())
    {
        PS2.insert(0, ' ');
        if (PS2.length() > 10)
        {
            break;
        }
    }
}

void Console::setBlockEnd(const QString &end)
{
    blockEnd = end;
}

void Console::newLine()
{
    insertPlainText(PS1 + " ");
}

void Console::newBlockLine()
{
    while (PS2.length() < PS1.length())
    {
        PS2.insert(0, ' ');
    }

    insertPlainText(PS2 + " ");
}

void Console::putOut(const QString &out)
{
    insertPlainText(out + ((*(out.end() - 1) == '\n') ? "" : "\n"));

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setLocalEchoEnabled(bool set)
{
    localEchoEnabled = set;
}

void Console::setSelfNewLineJudge(bool set)
{
    selfNewLineJudge = set;
}

void Console::completeNewLine()
{
    newLine();
}

void Console::incompleteNewLine()
{
    newBlockLine();
}

bool Console::isLastLine()
{
    return (blockCount() == textCursor().blockNumber()) ? true : false;
}

QMenu *Console::createContexMenu()
{
    QMenu *menu = new QMenu();
    menu->addAction(
        tr("Copy"), [this](void) { copy(); }, QKeySequence("Ctrl+C"));
    menu->addAction(
        tr("Paste"), [this](void) { paste(); }, QKeySequence("Ctrl+V"));
    menu->addSeparator();
    menu->addAction(
        tr("Clear"), [this](void) {setPlainText("");newLine(); }, QKeySequence("Ctrl+U"));
    menu->addAction(
        tr("Reset"), [this](void) {setPlainText("");emit reset();newLine(); }, QKeySequence("Ctrl+K"));

    return menu;
}

void Console::keyPressEvent(QKeyEvent *e)
{
    /* shortkey */
    if (e->modifiers() != Qt::NoModifier)
    {
        if (e->modifiers() == Qt::ControlModifier)
        {
            switch (e->key())
            {
            case Qt::Key_Left:
            case Qt::Key_Right:
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_C:
            case Qt::Key_V:
                QPlainTextEdit::keyPressEvent(e);
                break;
            case Qt::Key_Backspace:
                if (textCursor().columnNumber() > PS1.length() + 1 &&
                    toPlainText().split('\n').last().trimmed().length() > PS1.length())
                {
                    QPlainTextEdit::keyPressEvent(e);
                }
                break;
            case Qt::Key_U:
                setPlainText("");
                newLine();
                break;
            case Qt::Key_K:
                setPlainText("");
                emit reset();
                newLine();
                break;
            default:
                return;
            }
            return;
        }
        else if (e->modifiers() == Qt::ShiftModifier)
        {
            switch (e->key())
            {
            default:
                QPlainTextEdit::keyPressEvent(e);
                return;
            }
        }
        else if (e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
        {
            QPlainTextEdit::keyPressEvent(e);
            return;
        }
        return;
    }

    QStringList lines;
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
    {
        lines = toPlainText().split('\n');
    }

    switch (e->key())
    {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (!isLastLine())
        {
            moveCursor(QTextCursor::End);
        }
        moveCursor(QTextCursor::EndOfLine);
        QPlainTextEdit::keyPressEvent(e);
        if (selfNewLineJudge)
        {
            if (lines.last().endsWith(blockEnd))
            {
                newBlockLine();
            }
            else
            {
                newLine();
            }
        }
        emit newLineInput(lines.last().right(lines.last().length() - 2).trimmed());
        break;
    case Qt::Key_Backspace:
    case Qt::Key_Left:
        if (textCursor().columnNumber() > PS1.length() + 1)
        {
            QPlainTextEdit::keyPressEvent(e);
        }
        break;
    case Qt::Key_Right:
        QPlainTextEdit::keyPressEvent(e);
        break;
    case Qt::Key_Up:
    case Qt::Key_Down:
        moveCursor(QTextCursor::End);
        break;
    default:
        if (localEchoEnabled)
        {
            if (!isLastLine())
            {
                moveCursor(QTextCursor::End);
            }
            QPlainTextEdit::keyPressEvent(e);
        }
        emit newInput(e->text());
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    QPlainTextEdit::mousePressEvent(e);
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    QPlainTextEdit::mouseDoubleClickEvent(e);
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e);
    QMenu *menu = createContexMenu();
    menu->exec(e->globalPos());
    delete menu;
}
