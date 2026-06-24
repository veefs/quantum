#pragma once
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr, const QString &initialText = QString(), const QString &filePath = QString());

private:
    QPlainTextEdit *editor;
    QPlainTextEdit *console;
    QPushButton *run;
    QPushButton *save;
    QPushButton *refresh;
    QPushButton *help;
    QPushButton *clear;
    QString filePath;
    QString currentTicker;
    QLabel *chartLabel;
    QLabel *compareLabel;
    QLabel *logoLabel;
};