#include "TextBrowser.h"

TextBrowser::TextBrowser(QObject *parent, QTextBrowser *browser)
    : QObject(parent),
      browser(browser),
      dataType(TextType)
{
    if (browser == nullptr)
    {
        return;
    }
}

TextBrowser::~TextBrowser()
{
}

void TextBrowser::lock()
{
    mutex.lock();
    browser->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    cursor = browser->textCursor();
    scroll = browser->verticalScrollBar()->value();
    maxScroll = browser->verticalScrollBar()->maximum();
}
void TextBrowser::unlock()
{
    browser->setTextCursor(cursor);

    // auto scroll
    if (scroll == maxScroll)
    {
        browser->verticalScrollBar()->setValue(
            browser->verticalScrollBar()->maximum());
    }
    else
    {
        browser->verticalScrollBar()->setValue(scroll);
    }

    browser->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    mutex.unlock();
}

void TextBrowser::setDataType(DataType type)
{
    mutex.lock();
    dataType = type;
    mutex.unlock();
}

void TextBrowser::setFormat(int from, int end, QTextCharFormat format)
{
    lock();

    QTextCursor c = browser->textCursor();

    c.setPosition(from);
    c.setPosition(end, QTextCursor::KeepAnchor);

    browser->setTextCursor(c);
    browser->setCurrentCharFormat(format);
    browser->moveCursor(QTextCursor::End);

    unlock();
}

void TextBrowser::insertData(QByteArray *data)
{
    lock();

    browser->moveCursor(QTextCursor::End);

    if (dataType == TextType)
    {
        browser->insertPlainText(QString(*data));
    }
    else
    {
    }

    unlock();
}
