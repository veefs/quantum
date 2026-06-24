#include "SyntaxHighlighter.h"
#include <QRegularExpression>
#include <QTextCharFormat>

MyHighlighter::MyHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {}
ConsoleHighlighter::ConsoleHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {}

void MyHighlighter::highlightBlock(const QString &text) {
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(255, 121, 198, 255)); // pink - display

    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setForeground(QColor(139, 233, 253, 255)); // cyan - load

    QTextCharFormat contrastFormat;
    contrastFormat.setForeground(QColor(80, 250, 123, 255)); // green - compare

    QTextCharFormat popFormat;
    popFormat.setForeground(QColor(235, 45, 79, 255)); // red - range

    QTextCharFormat watchFormat;
    watchFormat.setForeground(QColor(255, 184, 108, 255)); // orange - watch/unwatch

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

    QRegularExpression contrastPattern("\\bcompare\\b");
    QRegularExpressionMatchIterator inzIt = contrastPattern.globalMatch(text);
    while (inzIt.hasNext()) {
        QRegularExpressionMatch match = inzIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), contrastFormat);
    }

    QRegularExpression popPattern("\\brange\\b");
    QRegularExpressionMatchIterator infIt = popPattern.globalMatch(text);
    while (infIt.hasNext()) {
        QRegularExpressionMatch match = infIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), popFormat);
    }

    QRegularExpression watchPattern("\\b(un)?watch\\b");
    QRegularExpressionMatchIterator watchIt = watchPattern.globalMatch(text);
    while (watchIt.hasNext()) {
        QRegularExpressionMatch match = watchIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), watchFormat);
    }
}

void ConsoleHighlighter::highlightBlock(const QString &text) {
    QTextCharFormat functionFormat;
    functionFormat.setForeground(QColor(189, 147, 249, 255)); // purple - FUNCTION

    QTextCharFormat slashFormat;
    slashFormat.setForeground(QColor(98, 114, 164, 255)); // comment gray - /

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(255, 121, 198, 255)); // pink - DISPLAY

    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setForeground(QColor(139, 233, 253, 255)); // cyan - LOAD

    QTextCharFormat contrastFormat;
    contrastFormat.setForeground(QColor(80, 250, 123, 255)); // green - COMPARE

    QTextCharFormat dataFormat;
    dataFormat.setForeground(QColor(255, 184, 108, 255)); // orange - DATA

    QTextCharFormat watchFormat;
    watchFormat.setForeground(QColor(241, 250, 140, 255)); // yellow - WATCH/UNWATCH

    QRegularExpression functionPattern("\\bFUNCTION\\b");
    QRegularExpressionMatchIterator funcIt = functionPattern.globalMatch(text);
    while (funcIt.hasNext()) {
        QRegularExpressionMatch match = funcIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), functionFormat);
    }

    QRegularExpression slashPattern("/");
    QRegularExpressionMatchIterator slashIt = slashPattern.globalMatch(text);
    while (slashIt.hasNext()) {
        QRegularExpressionMatch match = slashIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), slashFormat);
    }

    QRegularExpression displayPattern("\\bDISPLAY\\b");
    QRegularExpressionMatchIterator dispIt = displayPattern.globalMatch(text);
    while (dispIt.hasNext()) {
        QRegularExpressionMatch match = dispIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);
    }

    QRegularExpression loadPattern("\\bLOAD\\b");
    QRegularExpressionMatchIterator loadIt = loadPattern.globalMatch(text);
    while (loadIt.hasNext()) {
        QRegularExpressionMatch match = loadIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), preprocessorFormat);
    }

    QRegularExpression comparePattern("\\bCOMPARE\\b");
    QRegularExpressionMatchIterator compIt = comparePattern.globalMatch(text);
    while (compIt.hasNext()) {
        QRegularExpressionMatch match = compIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), contrastFormat);
    }

    QRegularExpression dataPattern("\\bDATA\\b");
    QRegularExpressionMatchIterator dataIt = dataPattern.globalMatch(text);
    while (dataIt.hasNext()) {
        QRegularExpressionMatch match = dataIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), dataFormat);
    }

    QRegularExpression watchPattern("\\b(UN)?WATCH\\b");
    QRegularExpressionMatchIterator watchIt = watchPattern.globalMatch(text);
    while (watchIt.hasNext()) {
        QRegularExpressionMatch match = watchIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), watchFormat);
    }
}