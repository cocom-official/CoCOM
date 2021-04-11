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

void Console::keyPressEvent(QKeyEvent *e)
{
    QStringList lines;
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
    {
        lines = toPlainText().split('\n');
    }

    switch (e->key())
    {
    case Qt::Key_Return:
    case Qt::Key_Enter:
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
        break;
    default:
        if (localEchoEnabled)
        {
            QPlainTextEdit::keyPressEvent(e);
        }
        emit newInput(e->text());
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    e->accept();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    e->accept();
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e);
    e->accept();
}
