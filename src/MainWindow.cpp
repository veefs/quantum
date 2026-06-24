#include "MainWindow.h"
#include "SyntaxHighlighter.h"
#include "CodeEditor.h"
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent, const QString &initialText, const QString &path) : QMainWindow(parent) {
    filePath = path;
    setWindowTitle("Quantum");
    resize(2400, 1200);

    setStyleSheet("background-color: #282a36;");

    // LOGO - far left
    logoLabel = new QLabel(this);
    QPixmap logoPixmap("resources/QST.png");
    logoLabel->setPixmap(logoPixmap.scaledToHeight(40, Qt::SmoothTransformation));
    logoLabel->resize(120, 40);
    logoLabel->move(0, 5);
    logoLabel->setStyleSheet("background-color: transparent;");

    editor = new CodeEditor(initialText, this);
    QFont font = editor->font();
    font.setPointSize(16);
    editor->setFont(font);
    editor->resize(800, 800);
    editor->move(0, 80);
    editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    editor->setStyleSheet(
        "QPlainTextEdit {"
        "  background-color: #282a36;"
        "  color: #f8f8f2;"
        "  border: 1px solid #44475a;"
        "  border-radius: 4px;"
        "  padding: 6px;"
        "}"
    );

    console = new QPlainTextEdit("", this);
    QFont consoleFont = console->font();
    consoleFont.setPointSize(16);
    console->setFont(consoleFont);
    console->resize(2400, 400);
    console->move(5, 850);
    console->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    console->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    console->setStyleSheet(
        "QPlainTextEdit {"
        "  background-color: #1e1f29;"
        "  color: #f8f8f2;"
        "  border: 1px solid #44475a;"
        "  border-radius: 4px;"
        "  padding: 6px;"
        "}"
    );

    MyHighlighter *highlighter = new MyHighlighter(editor->document());
    ConsoleHighlighter *consoleHighlighter = new ConsoleHighlighter(console->document());

    run = new QPushButton("run", this);
    save = new QPushButton("save", this);
    refresh = new QPushButton("refresh", this);
    help = new QPushButton("help", this);
    clear = new QPushButton("clear console", this);

    QString buttonStyle =
        "QPushButton {"
        "  background-color: transparent;"
        "  border: 1px solid #44475a;"
        "  border-radius: 6px;"
        "  outline: none;"
        "  padding: 0px;"
        "}"
        "QPushButton:hover {"
        "  background-color: transparent;"
        "  border: 1px solid #6272a4;"
        "}"
        "QPushButton:pressed {"
        "  background-color: transparent;"
        "  border: 1px solid #50fa7b;"
        "}"
        "QPushButton:focus {"
        "  outline: none;"
        "}";

    run->setStyleSheet(buttonStyle);
    save->setStyleSheet(buttonStyle);
    refresh->setStyleSheet(buttonStyle);
    help->setStyleSheet(buttonStyle);
    clear->setStyleSheet(buttonStyle);

    run->setFlat(true);
    save->setFlat(true);
    refresh->setFlat(true);
    help->setFlat(true);
    clear->setFlat(true);

    run->setIcon(QIcon("resources/buttons/run.png"));
    run->setIconSize(QSize(40, 40));
    run->setText("");

    save->setIcon(QIcon("resources/buttons/save.png"));
    save->setIconSize(QSize(40, 40));
    save->setText("");

    refresh->setIcon(QIcon("resources/buttons/refresh.png"));
    refresh->setIconSize(QSize(40, 40));
    refresh->setText("");

    clear->setIcon(QIcon("resources/buttons/clear.png"));
    clear->setIconSize(QSize(40, 40));
    clear->setText("");

    help->setIcon(QIcon("resources/buttons/help.png"));
    help->setIconSize(QSize(40, 40));
    help->setText("");

    chartLabel = new QLabel(this);
    chartLabel->resize(780, 750);
    chartLabel->move(810, 80);
    chartLabel->setScaledContents(true);
    chartLabel->setStyleSheet("background-color: #21222c; border: 1px solid #44475a; border-radius: 4px;");

    compareLabel = new QLabel(this);
    compareLabel->resize(780, 750);
    compareLabel->move(1600, 80);
    compareLabel->setScaledContents(true);
    compareLabel->setStyleSheet("background-color: #21222c; border: 1px solid #44475a; border-radius: 4px;");

    commandHandler = new CommandHandler(console, chartLabel, compareLabel, this);

    // RUN BUTTON
    run->resize(60, 60);
    run->move(140, 0);
    connect(run, &QPushButton::clicked, this, [this]() {
        console->appendPlainText("[LOAD] attempting to run: " + filePath);

        QString scriptText = editor->toPlainText();
        QStringList lines = scriptText.split('\n');

        commandHandler->resetRunState();

        for (const QString &rawLine : lines) {
            QString line = rawLine.trimmed();
            if (line.isEmpty()) continue;
            commandHandler->execute(line);
        }
    });

    // SAVE BUTTON
    save->resize(60, 60);
    save->move(220, 0);
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
    refresh->resize(60, 60);
    refresh->move(300, 0);
    connect(refresh, &QPushButton::clicked, this, [this]() {
        qDebug() << "Refresh clicked";

    });

    // HELP BUTTON
    help->resize(60, 60);
    help->move(380, 0);
    connect(help, &QPushButton::clicked, this, [this]() {
        qDebug() << "Help clicked";
    });

    clear->resize(60, 60);
    clear->move(460, 0);
    connect(clear, &QPushButton::clicked, this, [this]() {
        console->setPlainText("");
    });
}