#pragma once
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr, const QString &initialText = QString(), const QString &filePath = QString());

private:
    QPlainTextEdit *editor;
    QPushButton *run;
    QPushButton *save;
    QPushButton *refresh;
    QPushButton *help;
    QString filePath;
};
