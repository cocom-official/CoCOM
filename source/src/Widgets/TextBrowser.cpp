#include "TextBrowser.h"

TextBrowser::TextBrowser(QObject *parent, QTextBrowser *browser)
    : QObject(parent),
      browser(browser),
      dataType(TextType),
      encoding(DEFAULT_ENCODING),
      hexSeparator(' '),
      hexUpperCase(true),
      currentFindResultIndex(0)
{
    if (browser == nullptr)
    {
        throw std::runtime_error("browser should not be nullptr");
    }
    /* no line warp */
    // browser->setLineWrapMode(QTextEdit::NoWrap);

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

int TextBrowser::findHighlite(QString &text, int from)
{
    int count = 0;
    QTextDocument *document = browser->document();
    QTextCursor fromCursor;
    QTextCursor resultCursor;
    QTextCharFormat charFormatDefault;

    lock();

    fromCursor = browser->textCursor();
    fromCursor.setPosition(from);
    resultCursor = document->find(findText, fromCursor, QTextDocument::FindWholeWords);
    while (!resultCursor.isNull())
    {
        setFormat(resultCursor.selectionStart(), resultCursor.selectionEnd(), &charFormatDefault);
        resultCursor.clearSelection();
        resultCursor = document->find(findText, resultCursor, QTextDocument::FindWholeWords);
    }

    fromCursor = browser->textCursor();
    fromCursor.setPosition(from);
    resultCursor = document->find(text, fromCursor, QTextDocument::FindWholeWords);
    while (!resultCursor.isNull())
    {
        count++;
        findResultPos << resultCursor.position();
        setFormat(resultCursor.selectionStart(), resultCursor.selectionEnd(), getcharFormatHighlite());
        resultCursor.clearSelection();
        resultCursor = document->find(text, resultCursor, QTextDocument::FindWholeWords);
    }

    unlock();

    return count;
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

void TextBrowser::setEncoding(QString lEncoding)
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

void TextBrowser::setFindResultFormat(QTextCharFormat *format, int index)
{
    int findIndex = (index == -1) ? currentFindResultIndex : index;

    QTextCursor lCursor = browser->textCursor();
    int start = findResultPos[findIndex] - findText.length();
    int end = findResultPos[findIndex];

    lCursor.setPosition(start);
    lCursor.setPosition(end, QTextCursor::KeepAnchor);

    browser->setTextCursor(lCursor);
    browser->setCurrentCharFormat(*format);
    lCursor.setPosition(start);
    browser->setTextCursor(lCursor);
}

void TextBrowser::insertData(QByteArray data)
{
    QString string;

    if (dataType == TextType)
    {
        string = getEncodingCodecFromString(encoding)->toUnicode(data);
    }
    else
    {
        string = QString(data.toHex(hexSeparator).append(hexSeparator));
    }

    lock();

    QTextCharFormat defaultFormat;
    QTextCursor lCursor;
    int startPos = 0;
    int endPos = 0;

    browser->moveCursor(QTextCursor::End);
    lCursor = browser->textCursor();
    startPos = lCursor.position();

    if (hexUpperCase && dataType == HexType)
    {
        browser->insertPlainText(string.toUpper());
    }
    else
    {
        browser->insertPlainText(string);
    }

    browser->moveCursor(QTextCursor::End);
    lCursor = browser->textCursor();
    endPos = lCursor.position();

    lCursor.setPosition(startPos);
    lCursor.setPosition(endPos, QTextCursor::KeepAnchor);
    browser->setTextCursor(lCursor);
    browser->setCurrentCharFormat(defaultFormat);

    if (dataType == TextType)
    {
        processAllHintFormants();
    }

    unlock();
}

void TextBrowser::find(QString &text)
{
    int cur = 0;
    findResultPos.clear();
    currentFindResultIndex = 0;
    findResultCount = findHighlite(text);

    findText = text;
    if (findResultCount != 0)
    {
        mutex.lock();
        setFindResultFormat(getCharFormatHighliteSelect());
        mutex.unlock();
        cur = 1;
    }

    emit findResultChanged(cur, findResultCount);
}

void TextBrowser::findFromCurrent(QString &text)
{
    int count = findHighlite(text, findResultPos[currentFindResultIndex]);

    if (count != 0)
    {
        findResultCount += count;
    }
}

void TextBrowser::moveToNextFindResult()
{
    if (currentFindResultIndex + 1 == findResultCount)
    {
        findFromCurrent(findText);
    }

    if (0 == findResultCount)
    {
        return;
    }

    mutex.lock();

    QTextCursor lCursor = browser->textCursor();

    setFindResultFormat(getcharFormatHighlite());
    if (currentFindResultIndex + 1 == findResultCount)
    {
        currentFindResultIndex = 0;
    }
    else
    {
        currentFindResultIndex++;
    }
    lCursor.setPosition(findResultPos[currentFindResultIndex]);
    setFindResultFormat(getCharFormatHighliteSelect());

    mutex.unlock();

    emit findResultChanged(currentFindResultIndex + 1, findResultCount);
}

void TextBrowser::moveToPrivFindResult()
{
    if (0 == findResultCount)
    {
        return;
    }

    mutex.lock();

    QTextCursor lCursor = browser->textCursor();

    setFindResultFormat(getcharFormatHighlite());
    if (currentFindResultIndex == 0)
    {
        currentFindResultIndex = findResultCount - 1;
    }
    else
    {
        currentFindResultIndex--;
    }
    lCursor.setPosition(findResultPos[currentFindResultIndex]);
    setFindResultFormat(getCharFormatHighliteSelect());

    browser->setTextCursor(lCursor);

    mutex.unlock();

    emit findResultChanged(currentFindResultIndex + 1, findResultCount);
}

void TextBrowser::moveToLastFindResult()
{
    if (0 == findResultCount)
    {
        return;
    }

    int lastPos = findResultPos.last();

    mutex.lock();

    QTextCursor lCursor = browser->textCursor();

    setFindResultFormat(getcharFormatHighlite());
    lCursor.setPosition(lastPos);
    setFindResultFormat(getCharFormatHighliteSelect(), findResultPos.size() - 1);

    browser->setTextCursor(lCursor);

    mutex.unlock();

    emit findResultChanged(findResultCount, findResultCount);
}

void TextBrowser::moveToFirstFindResult()
{
    if (0 == findResultCount)
    {
        return;
    }

    int firstPos = findResultPos.first();

    mutex.lock();

    QTextCursor lCursor = browser->textCursor();

    setFindResultFormat(getcharFormatHighlite());
    lCursor.setPosition(firstPos);
    setFindResultFormat(getCharFormatHighliteSelect(), 0);

    browser->setTextCursor(lCursor);

    mutex.unlock();

    emit findResultChanged(1, findResultCount);
}

void TextBrowser::clear()
{
    lock();

    browser->clear();
    resetHintFormant(&successHint);
    resetHintFormant(&warnHint);
    resetHintFormant(&errorHint);
    findResultCount = 0;
    currentFindResultIndex = 0;
    findResultPos.clear();

    emit findResultChanged(0, 0);

    unlock();
}
