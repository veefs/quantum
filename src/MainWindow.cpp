#include "MainWindow.h"
#include "SyntaxHighlighter.h"
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent, const QString &initialText, const QString &path) : QMainWindow(parent) {
    filePath = path;
    setWindowTitle("Quantum");
    resize(2000, 1200);

    setStyleSheet("background-color: #282a36;");

    // LOGO - far left
    logoLabel = new QLabel(this);
    QPixmap logoPixmap("resources/QST.png");
    logoLabel->setPixmap(logoPixmap.scaledToHeight(40, Qt::SmoothTransformation));
    logoLabel->resize(120, 40);
    logoLabel->move(0, 5);
    logoLabel->setStyleSheet("background-color: transparent;");

    editor = new QPlainTextEdit(initialText, this);
    QFont font = editor->font();
    font.setPointSize(16);
    editor->setFont(font);
    editor->resize(800, 800);
    editor->move(0, 50);
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
    console->resize(2000, 400);
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
        "  background-color: #44475a;"
        "  color: #f8f8f2;"
        "  border: none;"
        "  border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #6272a4;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #50fa7b;"
        "  color: #282a36;"
        "}";

    run->setStyleSheet(buttonStyle);
    save->setStyleSheet(buttonStyle);
    refresh->setStyleSheet(buttonStyle);
    help->setStyleSheet(buttonStyle);
    clear->setStyleSheet(buttonStyle);

    chartLabel = new QLabel(this);
    chartLabel->resize(580, 800);
    chartLabel->move(810, 50);
    chartLabel->setScaledContents(true);
    chartLabel->setStyleSheet("background-color: #21222c; border: 1px solid #44475a; border-radius: 4px; color: #6272a4;");
    chartLabel->setAlignment(Qt::AlignCenter);
    chartLabel->setText("No chart loaded");

    compareLabel = new QLabel(this);
    compareLabel->resize(580, 800);
    compareLabel->move(1400, 50);
    compareLabel->setScaledContents(true);
    compareLabel->setStyleSheet("background-color: #21222c; border: 1px solid #44475a; border-radius: 4px; color: #6272a4;");
    compareLabel->setAlignment(Qt::AlignCenter);
    compareLabel->setText("No chart loaded");

    // RUN BUTTON - shifted right to make room for logo
    run->resize(100, 50);
    run->move(130, 0);
    connect(run, &QPushButton::clicked, this, [this]() {

        console->appendPlainText("[LOAD] attempting to run: " + filePath);


        QString scriptText = editor->toPlainText();
        QStringList lines = scriptText.split('\n');

        bool load_triggered = false;

        for (const QString &rawLine : lines) {
            QString line = rawLine.trimmed();
            if (line.isEmpty()) continue;

            int openParen = line.indexOf('(');
            int closeParen = line.lastIndexOf(')');

            if (openParen == -1 || closeParen == -1 || closeParen < openParen) {
                qDebug() << "Malformed line, skipping:" << line;
                continue;
            }

            QString command = line.left(openParen).trimmed();
            QString argument = line.mid(openParen + 1, closeParen - openParen - 1).trimmed();

            if (command == "load") {
                if (argument.isEmpty()) {

                    console->appendPlainText("[ERROR] LOAD requires a ticker argument");

                } else {
                    load_triggered = true;
                    currentTicker = argument;

                    console->appendPlainText("[FUNCTION/LOAD] loading " + argument);

                    QProcess process;
                    process.start("python", QStringList() << "scripts/fetchPrice.py" << argument);
                    process.waitForFinished();

                    QString output = process.readAllStandardOutput();
                    QString errorOutput = process.readAllStandardError();

                    if (!errorOutput.isEmpty()) {
                        qDebug() << "Python error:" << errorOutput;
                    } else {
                        console->appendPlainText("[DATA/LOAD] running script/fetchPrice.py " + argument);
                        console->appendPlainText(output);
                    }
                }
            }
            else if (command == "display") {
                if (load_triggered || !currentTicker.isEmpty()) {

                    console->appendPlainText("[FUNCTION/DISPLAY] displaying: " +  currentTicker);

                    QProcess process;
                    process.start("python", QStringList() << "scripts/fetchPrice.py" << currentTicker << "-d");
                    process.waitForFinished();

                    QString output = process.readAllStandardOutput();
                    QString errorOutput = process.readAllStandardError();

                    if (!errorOutput.isEmpty()) {
                        qDebug() << "Python error:" << errorOutput;
                    } else {
                        console->appendPlainText("[FUNCTION/DISPLAY] running scripts/fetchPrice.py " + currentTicker + " -d");
                    }

                    QString imagePath = currentTicker + "_history.png";
                    QPixmap pixmap(imagePath);
                    if (pixmap.isNull()) {
                        qDebug() << "Failed to load image:" << imagePath;
                        chartLabel->setText("Failed to load chart");
                    } else {
                        chartLabel->setPixmap(pixmap);
                        console->appendPlainText("Loaded chart image: " + imagePath);
                    }
                } else {
                    qDebug() << "ERROR: LOAD command not triggered";
                }
            }

            else if (command == "compare") {
                if (load_triggered || !currentTicker.isEmpty()) {

                    console->appendPlainText("[FUNCTION/COMPARE] loaded:" + currentTicker + " vs. " + argument);


                     QProcess process;
                    process.start("python", QStringList() << "scripts/fetchPrice.py" << argument << "-d");
                    process.waitForFinished();

                    QString output = process.readAllStandardOutput();
                    QString errorOutput = process.readAllStandardError();

                    if (!errorOutput.isEmpty()) {
                        qDebug() << "Python error:" << errorOutput;
                    } else {
                        console->appendPlainText("[DATA/COMPARE] script/fetchPrice.py " + argument + " -d");
                        console->appendPlainText(output);
                    }

                    QString imagePath = argument + "_history.png";
                    QPixmap pixmap(imagePath);
                    if (pixmap.isNull()) {
                        qDebug() << "Failed to load image:" << imagePath;
                        compareLabel->setText("Failed to load chart");
                    } else {
                        compareLabel->setPixmap(pixmap);
                        qDebug() << "Loaded chart image:" << imagePath;
                        console->appendPlainText("[FUNCTION/COMPARE] loaded image: " + imagePath);
                    }

                } else {
                    qDebug() << "ERROR: LOAD command not triggered";
                }
            }

            else {
                qDebug() << "Unknown command:" << command;
            }
        }
    });

    // SAVE BUTTON
    save->resize(100, 50);
    save->move(230, 0);
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
    refresh->move(330, 0);
    connect(refresh, &QPushButton::clicked, this, [this]() {
        qDebug() << "Refresh clicked";

    });

    // HELP BUTTON
    help->resize(100, 50);
    help->move(430, 0);
    connect(help, &QPushButton::clicked, this, [this]() {
        qDebug() << "Help clicked";
    });

    clear->resize(100, 50);
    clear->move(530, 0);
    connect(clear, &QPushButton::clicked, this, [this]() {
        console->setPlainText("");
    });
}