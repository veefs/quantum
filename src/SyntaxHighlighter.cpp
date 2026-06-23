#include "SyntaxHighlighter.h"
#include <QRegularExpression>
#include <QTextCharFormat>

MyHighlighter::MyHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {}

void MyHighlighter::highlightBlock(const QString &text) {
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(136, 57, 239, 255));

    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setForeground(QColor(4, 165, 229, 255));

    QRegularExpression intPattern("\\bdisplay\\b");
    QRegularExpressionMatchIterator intIt = intPattern.globalMatch(text);
    while (intIt.hasNext()) {
        QRegularExpressionMatch match = intIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);
    }

    QRegularExpression includePattern("\\bload\\b");
    QRegularExpressionMatchIterator incIt = includePattern.globalMatch(text);
    while (incIt.hasNext()) {
        QRegularExpressionMatch match = incIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), preprocessorFormat);
    }
}