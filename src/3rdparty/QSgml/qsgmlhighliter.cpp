
#include "qsgmlhighliter.h"

QSgmlHighliter::QSgmlHighliter(QTextDocument * parent)
    : QSyntaxHighlighter(parent)
{
}

void QSgmlHighliter::highlightBlock(const QString &text)
{
    int index;
    QString pattern;
    QTextCharFormat myClassFormat;
    QRegExp expression;

    expression.setMinimal(false);
    myClassFormat.setForeground(Qt::blue);
    pattern = "<[/!]*[\\w]+";
    expression.setPattern(pattern);
    index = text.indexOf(expression);
    while (index >= 0)
    {
        int length = expression.matchedLength();
        setFormat(index, length, myClassFormat);
        index = text.indexOf(expression, index + length);
    }

    expression.setMinimal(false);
    myClassFormat.setForeground(Qt::blue);
    pattern = ">";
    expression.setPattern(pattern);
    index = text.indexOf(expression);
    while (index >= 0)
    {
        int length = expression.matchedLength();
        setFormat(index, length, myClassFormat);
        index = text.indexOf(expression, index + length);
    }

    expression.setMinimal(true);
    myClassFormat.setForeground(Qt::darkMagenta);
    pattern = "[\"'].*[\"']";
    expression.setPattern(pattern);
    index = text.indexOf(expression);
    while (index >= 0)
    {
        int length = expression.matchedLength();
        setFormat(index, length, myClassFormat);
        index = text.indexOf(expression, index + length);
    }

    expression.setMinimal(true);
    myClassFormat.setForeground(Qt::darkGreen);
    pattern = "<!--.*-->";
    expression.setPattern(pattern);
    index = text.indexOf(expression);
    while (index >= 0)
    {
        int length = expression.matchedLength();
        setFormat(index, length, myClassFormat);
        index = text.indexOf(expression, index + length);
    }

    expression.setMinimal(true);
    myClassFormat.setForeground(Qt::darkYellow);
    pattern = "[\\w-_]+[\\s]*=";
    expression.setPattern(pattern);
    index = text.indexOf(expression);
    while (index >= 0)
    {
        int length = expression.matchedLength();
        setFormat(index, length, myClassFormat);
        index = text.indexOf(expression, index + length);
    }
}
