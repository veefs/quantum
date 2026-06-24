#include "Command.h"
#include <QProcess>
#include <QPixmap>
#include <QPixmapCache>
#include <QDebug>

CommandHandler::CommandHandler(QPlainTextEdit *console, QLabel *chartLabel, QLabel *compareLabel, QObject *parent)
    : QObject(parent), console(console), chartLabel(chartLabel), compareLabel(compareLabel) {

    watchTimer = new QTimer(this);
    connect(watchTimer, &QTimer::timeout, this, &CommandHandler::onWatchTick);

    watchProcess = new QProcess(this);
    connect(watchProcess, &QProcess::finished, this, &CommandHandler::onWatchProcessFinished);
}

void CommandHandler::resetRunState() {
    loadTriggered = false;
}

QStringList CommandHandler::splitArgs(const QString &rawArgument) {
    QStringList parts = rawArgument.split(',', Qt::SkipEmptyParts);
    for (QString &part : parts) {
        part = part.trimmed();
    }
    return parts;
}

void CommandHandler::execute(const QString &line) {
    int openParen = line.indexOf('(');
    int closeParen = line.lastIndexOf(')');

    if (openParen == -1 || closeParen == -1 || closeParen < openParen) {
        qDebug() << "Malformed line, skipping:" << line;
        return;
    }

    QString command = line.left(openParen).trimmed();
    QString rawArgument = line.mid(openParen + 1, closeParen - openParen - 1).trimmed();
    QStringList args = splitArgs(rawArgument);

    if (command == "load") handleLoad(args);
    else if (command == "display") handleDisplay(args);
    else if (command == "compare") handleCompare(args);
    else if (command == "range") handleRange(args);
    else if (command == "watch") handleWatch(args);
    else if (command == "unwatch") handleUnwatch(args);
    else handleUnknown(command);
}

QString CommandHandler::runFetchScript(const QStringList &scriptArgs, const QString &logPrefix) {
    QProcess process;
    process.start("python", QStringList() << "scripts/fetchPrice.py" << scriptArgs);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QString errorOutput = process.readAllStandardError();

    if (!errorOutput.isEmpty()) {
        qDebug() << "Python error:" << errorOutput;
        console->appendPlainText("[ERROR] " + logPrefix + " failed: " + errorOutput);
    } else {
        console->appendPlainText(logPrefix);
    }

    return output;
}

void CommandHandler::loadChartImage(QLabel *targetLabel, const QString &ticker, const QString &range) {
    QString imagePath = "resources/" + ticker + "_" + range + "_history.png";
    QPixmap pixmap(imagePath);

    if (pixmap.isNull()) {
        qDebug() << "Failed to load image:" << imagePath;
        targetLabel->setText("Failed to load chart");
    } else {
        targetLabel->setPixmap(pixmap);
        console->appendPlainText("Loaded chart image: " + imagePath);
    }
}

void CommandHandler::loadWatchImage(QLabel *targetLabel, const QString &ticker) {
    QString imagePath = "resources/" + ticker + "_watch.png";

    // QPixmap caches by file path internally (QPixmapCache). Since watch()
    // overwrites this exact same path every tick, Qt can serve a stale
    // cached copy instead of re-reading the freshly-written file from disk.
    QPixmapCache::remove(imagePath);

    QPixmap pixmap;
    pixmap.load(imagePath);

    if (pixmap.isNull()) {
        qDebug() << "Failed to load image:" << imagePath;
        targetLabel->setText("Failed to load chart");
    } else {
        targetLabel->setPixmap(pixmap);
        console->appendPlainText("Loaded chart image: " + imagePath);
    }
}

void CommandHandler::handleLoad(const QStringList &args) {
    if (args.isEmpty()) {
        console->appendPlainText("[ERROR] LOAD requires a ticker argument");
        return;
    }

    currentTicker = args[0];
    if (args.size() > 1) {
        currentRange = args[1];
    }
    loadTriggered = true;

    console->appendPlainText("[FUNCTION/LOAD] loading " + currentTicker + " (" + currentRange + ")");
    runFetchScript({currentTicker, currentRange}, "[DATA/LOAD] fetched " + currentTicker);
}

void CommandHandler::handleDisplay(const QStringList & /*args*/) {
    if (!loadTriggered && currentTicker.isEmpty()) {
        console->appendPlainText("[ERROR] LOAD command not triggered");
        return;
    }

    console->appendPlainText("[FUNCTION/DISPLAY] displaying: " + currentTicker);
    runFetchScript({currentTicker, currentRange, "-d"},
                    "[FUNCTION/DISPLAY] rendered " + currentTicker);
    loadChartImage(chartLabel, currentTicker, currentRange);
}

void CommandHandler::handleCompare(const QStringList &args) {
    if (!loadTriggered && currentTicker.isEmpty()) {
        console->appendPlainText("[ERROR] LOAD command not triggered");
        return;
    }
    if (args.isEmpty()) {
        console->appendPlainText("[ERROR] COMPARE requires a ticker argument");
        return;
    }

    QString compareTicker = args[0];
    QString compareRange = args.size() > 1 ? args[1] : currentRange;

    console->appendPlainText("[FUNCTION/COMPARE] " + currentTicker + " vs. " + compareTicker);
    runFetchScript({compareTicker, compareRange, "-d"},
                    "[DATA/COMPARE] fetched " + compareTicker);
    loadChartImage(compareLabel, compareTicker, compareRange);
}

void CommandHandler::handleRange(const QStringList &args) {
    currentRange = args.isEmpty() ? "2y" : args[0];
    console->appendPlainText("[FUNCTION/RANGE] set to " + currentRange);
}

void CommandHandler::handleWatch(const QStringList &args) {
    watchTicker = args.isEmpty() ? currentTicker : args[0];

    if (watchTicker.isEmpty()) {
        console->appendPlainText("[ERROR] WATCH requires a ticker (or a prior LOAD)");
        return;
    }

    int pollSeconds = (args.size() > 1) ? args[1].toInt() : 30;
    if (pollSeconds <= 0) pollSeconds = 30;

    watchWindowMinutes = (args.size() > 2) ? args[2].toInt() : 60;
    if (watchWindowMinutes <= 0) watchWindowMinutes = 60;

    console->appendPlainText(
        "[FUNCTION/WATCH] watching " + watchTicker +
        " every " + QString::number(pollSeconds) + "s"
    );

    onWatchTick();
    watchTimer->start(pollSeconds * 1000);
}

void CommandHandler::handleUnwatch(const QStringList & /*args*/) {
    bool wasActive = watchTimer->isActive();
    watchTimer->stop();

    if (watchProcess->state() != QProcess::NotRunning) {
        watchProcess->kill();
    }

    if (!wasActive) {
        console->appendPlainText("[FUNCTION/UNWATCH] nothing is being watched");
    } else {
        console->appendPlainText("[FUNCTION/UNWATCH] stopped watching " + watchTicker);
    }
}

// Non-blocking: just kicks off the python process and returns immediately.
// If a previous tick's process is somehow still running, skip this tick
// rather than overlapping two runs against the same QProcess.
void CommandHandler::onWatchTick() {
    if (watchProcess->state() != QProcess::NotRunning) {
        qDebug() << "Watch tick skipped - previous run still in progress";
        return;
    }

    console->appendPlainText("[FUNCTION/WATCH] refreshing " + watchTicker);
    watchProcess->start("python", QStringList()
        << "scripts/watchPrice.py"
        << watchTicker
        << QString::number(watchWindowMinutes));
}

// Runs on the GUI thread, but only once the subprocess has already
// finished in the background - nothing here blocks waiting for python.
void CommandHandler::onWatchProcessFinished(int /*exitCode*/, QProcess::ExitStatus /*status*/) {
    QString errorOutput = watchProcess->readAllStandardError();

    if (!errorOutput.isEmpty()) {
        qDebug() << "Python error:" << errorOutput;
        console->appendPlainText("[ERROR] watch refresh failed: " + errorOutput);
        return;
    }

    console->appendPlainText("[DATA/WATCH] refreshed " + watchTicker);
    loadWatchImage(chartLabel, watchTicker);
}

void CommandHandler::handleUnknown(const QString &command) {
    qDebug() << "Unknown command:" << command;
    console->appendPlainText("[ERROR] unknown command: " + command);
}
