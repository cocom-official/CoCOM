#include "TextBrowser.h"

TextBrowser::TextBrowser(QObject *parent, QTextBrowser *browser)
    : QObject(parent),
      browser(browser),
      dataType(TextType),
      encoding(LocalEnc)
{
    if (browser == nullptr)
    {
        throw std::runtime_error("browser should not be nullptr");
    }
    browser->insertPlainText(QString(QByteArray("\r\n")));

    initHintFormat();
}

TextBrowser::~TextBrowser()
{
}

void TextBrowser::lock()
{
    mutex.lock();
    browser->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    lockCursor = browser->textCursor();
    scroll = browser->verticalScrollBar()->value();
    maxScroll = browser->verticalScrollBar()->maximum();
}
void TextBrowser::unlock()
{
    browser->setTextCursor(lockCursor);

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

void TextBrowser::addHintFormat(HintFormat *hintFormat, const QString *key, QTextCharFormat *format)
{
    Q_ASSERT(hintFormat);
    Q_ASSERT(key);
    Q_ASSERT(format);

    HintKey *hintKey = new HintKey();
    hintKey->key = key;
    hintKey->cursorPos = 0;

    hintFormat->key << hintKey;
    hintFormat->format = format;
}

void TextBrowser::initHintFormat()
{
    QTextCharFormat *format = nullptr;

    format = new QTextCharFormat();
    format->setFontUnderline(true);
    format->setForeground(QBrush(Qt::GlobalColor::green));
    for (auto &&i : successKey)
    {
        addHintFormat(&successHint, &i, format);
    }

    format = new QTextCharFormat();
    format->setFontUnderline(true);
    format->setForeground(QBrush(Qt::GlobalColor::yellow));
    for (auto &&i : warnKey)
    {
        addHintFormat(&warnHint, &i, format);
    }

    format = new QTextCharFormat();
    format->setFontUnderline(true);
    format->setForeground(QBrush(Qt::GlobalColor::red));
    for (auto &&i : errorKey)
    {
        addHintFormat(&errorHint, &i, format);
    }
}

void TextBrowser::processHintFormant(HintFormat *hint)
{
    QTextDocument *document = browser->document();
    browser->moveCursor(QTextCursor::End);
    QTextCursor lCursor;

    for (auto &&i : hint->key)
    {
        lCursor = browser->textCursor();
        lCursor.setPosition(i->cursorPos);
        lCursor = document->find(*(i->key), lCursor, QTextDocument::FindWholeWords);
        while (!lCursor.isNull())
        {
            setFormat(lCursor.selectionStart(), lCursor.selectionEnd(), hint->format);
            i->cursorPos = lCursor.position();
            lCursor.clearSelection();
            lCursor = document->find(*(i->key), lCursor, QTextDocument::FindWholeWords);
        }
        browser->moveCursor(QTextCursor::End);
        int pos = browser->textCursor().position() - i->key->length() - 1;
        i->cursorPos = (pos > 0) ? pos : 0;
    }
}

void TextBrowser::processAllHintFormants()
{
    processHintFormant(&successHint);
    processHintFormant(&warnHint);
    processHintFormant(&errorHint);
}

void TextBrowser::resetHintFormant(HintFormat *hint)
{
    for (auto &&i : hint->key)
    {
        i->cursorPos = 0;
    }
}

void TextBrowser::setDataType(DataType type)
{
    mutex.lock();
    dataType = type;
    mutex.unlock();
}

void TextBrowser::setEncoding(EncodingType lEncoding)
{
    encoding = lEncoding;
}

void TextBrowser::setFormat(int start, int end, QTextCharFormat *format)
{
    QTextCursor lCursor = browser->textCursor();

    lCursor.setPosition(start);
    lCursor.setPosition(end, QTextCursor::KeepAnchor);

    browser->setTextCursor(lCursor);
    browser->setCurrentCharFormat(*format);
}

void TextBrowser::insertData(QByteArray *data)
{
    QTextCodec *codec = nullptr;

    switch (encoding)
    {
    case UTF_8:
        codec = QTextCodec::codecForName("UTF-8");
        break;

    default:
        codec = QTextCodec::codecForLocale();
        break;
    }
    QString string = codec->toUnicode(*data);

    lock();

    if (dataType == TextType)
    {
        QTextCharFormat defaultFormat;
        QTextCursor lCursor;
        int startPos = 0;
        int endPos = 0;

        browser->moveCursor(QTextCursor::End);
        lCursor = browser->textCursor();
        startPos = lCursor.position();

        browser->insertPlainText(string);

        browser->moveCursor(QTextCursor::End);
        lCursor = browser->textCursor();
        endPos = lCursor.position();

        lCursor.setPosition(startPos);
        lCursor.setPosition(endPos, QTextCursor::KeepAnchor);
        browser->setTextCursor(lCursor);
        browser->setCurrentCharFormat(defaultFormat);

        processAllHintFormants();
    }
    else
    {
    }

    unlock();
}

void TextBrowser::clear()
{
    lock();

    browser->clear();
    resetHintFormant(&successHint);
    resetHintFormant(&warnHint);
    resetHintFormant(&errorHint);

    unlock();
}
