
#ifndef QSGMLHIGHLITER_H
#define QSGMLHIGHLITER_H

#include <QSyntaxHighlighter>

class QSgmlHighliter : public QSyntaxHighlighter
{
public:
    QSgmlHighliter(QTextDocument * parent);
private:
    void highlightBlock(const QString &text);
};

#endif // QSGMLHIGHLITER_H
