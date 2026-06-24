#pragma once
#include <QPlainTextEdit>
#include <QWidget>

class LineNumberArea;

// CodeEditor extends QPlainTextEdit purely to paint a visual-only line
// number gutter. The gutter is never part of the document, so
// toPlainText() / save logic is completely unaffected.
class CodeEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit CodeEditor(const QString &initialText, QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
};

class LineNumberArea : public QWidget {
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor) {}

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};
