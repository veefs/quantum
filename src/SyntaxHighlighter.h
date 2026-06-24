#pragma once
#include <QSyntaxHighlighter>
#include <QTextDocument>

class MyHighlighter : public QSyntaxHighlighter {
public:
    MyHighlighter(QTextDocument *parent);

protected:
    void highlightBlock(const QString &text) override;
};

class ConsoleHighlighter : public QSyntaxHighlighter {
public:
    ConsoleHighlighter(QTextDocument *parent);

protected:
    void highlightBlock(const QString &text) override;
};