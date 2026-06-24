#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QPlainTextEdit>
#include <QLabel>
#include <QTimer>
#include <QProcess>

// CommandHandler owns all script-command logic (load, display, compare,
// range, watch, etc). MainWindow just feeds it lines and it does the rest -
// running python, updating the console, and loading chart images.
//
// To add a new command:
//   1. Add a case in execute() for the new command name
//   2. Write a handleX(...) method for it
//   3. Done - MainWindow.cpp does not need to change at all
//
// QObject is required (not a plain class) because watch()/unwatch() use a
// QTimer, which needs the signal/slot system to fire on a schedule.
class CommandHandler : public QObject {
    Q_OBJECT
public:
    CommandHandler(QPlainTextEdit *console, QLabel *chartLabel, QLabel *compareLabel, QObject *parent = nullptr);

    // Parses and executes a single line like "load(AMZN, 2mo)".
    // Call this once per non-empty line in the script.
    void execute(const QString &line);

    // Resets per-run state (currently just loadTriggered).
    // Call this once before running a whole script.
    void resetRunState();

private slots:
    // Fired by watchTimer on each interval tick. Kicks off an async
    // watchPrice.py run rather than blocking on it.
    void onWatchTick();

    // Fired when the async watch process finishes. This is where the
    // chart image actually gets loaded - never on the GUI thread's dime.
    void onWatchProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
    QPlainTextEdit *console;
    QLabel *chartLabel;
    QLabel *compareLabel;

    QString currentTicker;
    QString currentRange = "2y";
    bool loadTriggered = false;

    QTimer *watchTimer;
    QProcess *watchProcess;        // persistent, reused process for watch ticks
    QString watchTicker;
    int watchWindowMinutes = 60;   // how many recent 1m bars watch() plots

    // Splits "AMZN, 2mo" into ["AMZN", "2mo"], trimmed, empty entries skipped.
    QStringList splitArgs(const QString &rawArgument);

    void handleLoad(const QStringList &args);
    void handleDisplay(const QStringList &args);
    void handleCompare(const QStringList &args);
    void handleRange(const QStringList &args);
    void handleWatch(const QStringList &args);
    void handleUnwatch(const QStringList &args);
    void handleMonte(const QStringList &args);
    void handleUnknown(const QString &command);

    // Shared helper: runs fetchPrice.py with given args, returns stdout.
    // Logs errors to console automatically. Blocking - only used for
    // one-shot, user-triggered commands (load/display/compare), never
    // for anything on a timer.
    QString runFetchScript(const QStringList &scriptArgs, const QString &logPrefix);

    // Shared helper: loads "resources/{ticker}_{range}_history.png" into a label.
    void loadChartImage(QLabel *targetLabel, const QString &ticker, const QString &range);

    // Shared helper: loads "resources/{ticker}_watch.png" into a label.
    void loadWatchImage(QLabel *targetLabel, const QString &ticker);

    // Shared helper: loads "resources/{tickers joined by _}_monte.png" into a label.
    void loadMonteImage(QLabel *targetLabel, const QString &tickerLabel);
};
