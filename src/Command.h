#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QPlainTextEdit>
#include <QLabel>
#include <QTimer>
#include <QProcess>

// Owns all script-command logic (load, display, compare, range, watch,
// monte). MainWindow just feeds it lines; to add a command, add a case
// in execute() and a handleX() method - MainWindow.cpp doesn't change.
class CommandHandler : public QObject {
    Q_OBJECT
public:
    CommandHandler(QPlainTextEdit *console, QLabel *chartLabel, QLabel *compareLabel, QObject *parent = nullptr);

    void execute(const QString &line);
    void resetRunState();

private slots:
    void onWatchTick();
    void onWatchProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
    QPlainTextEdit *console;
    QLabel *chartLabel;
    QLabel *compareLabel;

    QString currentTicker;
    QString currentRange = "2y";
    bool loadTriggered = false;

    QTimer *watchTimer;
    QProcess *watchProcess;
    QString watchTicker;
    int watchWindowMinutes = 60;

    QStringList splitArgs(const QString &rawArgument);

    void handleLoad(const QStringList &args);
    void handleDisplay(const QStringList &args);
    void handleCompare(const QStringList &args);
    void handleRange(const QStringList &args);
    void handleWatch(const QStringList &args);
    void handleUnwatch(const QStringList &args);
    void handleMonte(const QStringList &args);
    void handleUnknown(const QString &command);

    // Blocking - only for one-shot commands, never anything on a timer.
    QString runFetchScript(const QStringList &scriptArgs, const QString &logPrefix);

    void loadChartImage(QLabel *targetLabel, const QString &ticker, const QString &range);
    void loadWatchImage(QLabel *targetLabel, const QString &ticker);
    void loadMonteImage(QLabel *targetLabel, const QString &tickerLabel);
};
