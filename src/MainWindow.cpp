#include "MainWindow.h"
#include "SyntaxHighlighter.h"
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent, const QString &initialText, const QString &path) : QMainWindow(parent) {
    filePath = path;
    setWindowTitle("Quantum");
    resize(1200, 800);

    editor = new QPlainTextEdit(initialText, this);
    QFont font = editor->font();
    font.setPointSize(16);
    editor->setFont(font);
    editor->resize(800, 800);
    editor->move(0, 50);
    editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    MyHighlighter *highlighter = new MyHighlighter(editor->document());

    run = new QPushButton("run", this);
    save = new QPushButton("save", this);
    refresh = new QPushButton("refresh", this);
    help = new QPushButton("help", this);

    // RUN BUTTON
    run->resize(100, 50);
    run->move(0, 0);
    connect(run, &QPushButton::clicked, this, [this]() {

        QString scriptText = editor->toPlainText();

        QString currentLine; 
        QStringList lines;    

    for (QChar c : scriptText) {
            if (c == '\n') {
                lines.append(currentLine);
                currentLine.clear();
            } else {
                currentLine += c;
            }
        }
        if (!currentLine.isEmpty()) {
            lines.append(currentLine);
        }

        for (const QString &line : lines) {
            qDebug() << "LINE:" << line;
        }
    });

    // SAVE BUTTON
    save->resize(100, 50);
    save->move(100, 0);
    connect(save, &QPushButton::clicked, this, [this]() {
         QString saveText = editor->toPlainText();
         if (!filePath.isEmpty()) {
             QFile outFile(filePath);
             if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                 QTextStream stream(&outFile);
                 stream.setEncoding(QStringConverter::Utf8);
                 stream << saveText;
                 outFile.close();
                 qDebug() << "Saved to:" << filePath;
             } else {
                 qDebug() << "Failed to open file for writing:" << filePath;
             }
         } else {
             qDebug() << "No file path set, cannot save.";
         }
    });

    // REFRESH BUTTON
    refresh->resize(100, 50);
    refresh->move(200, 0);
    connect(refresh, &QPushButton::clicked, this, [this]() {
        qDebug() << "Refresh clicked";
    });

    // HELP BUTTON
    help->resize(100, 50);
    help->move(300, 0);
    connect(help, &QPushButton::clicked, this, [this]() {
        qDebug() << "Help clicked";
    });
}
