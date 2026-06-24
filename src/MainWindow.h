#pragma once
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include "CodeEditor.h"
#include "Command.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr, const QString &initialText = QString(), const QString &filePath = QString());

private:
    CodeEditor *editor;
    QPlainTextEdit *console;
    QPushButton *run;
    QPushButton *save;
    QPushButton *refresh;
    QPushButton *help;
    QPushButton *clear;
    QString filePath;
    QLabel *chartLabel;
    QLabel *compareLabel;
    QLabel *logoLabel;
    CommandHandler *commandHandler;
};