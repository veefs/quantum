# Quantum

A lightweight IDE for quants. Write a small script in Quantum's own
language (`.qtm` files), hit Run, and get live charts, comparisons, and
Monte Carlo risk simulations rendered straight into the editor - no
Jupyter notebook required.

![Quantum logo](/resources/qst.png)

## Why

Most quant workflows live in notebooks: re-running cells, re-importing
yfinance, copy-pasting boilerplate just to look at a price chart. Quantum
strips that down to one line per action:

```
load(AMZN)
display()
compare(AAPL)
```

Press Run. That's it.

## Features

- **Custom scripting language** (`.qtm`) - one command per line, plain
  English-ish syntax, no Python boilerplate required to use it
- **Dracula-themed code editor** with syntax highlighting and line numbers
- **Live charts** rendered inline via matplotlib, driven by `yfinance`
- **Side-by-side comparison** of two tickers
- **Live-updating watch mode** that polls a ticker on a timer without
  freezing the UI
- **Monte Carlo portfolio simulation** with VaR/CVaR across four models
  (historical, normal, t-distribution, and simulated)
- **Console log** with its own syntax highlighting for quick scanning

## Commands

See [`docs/functions.md`](docs/functions.md) for full syntax and options.
Quick reference:

| Command | What it does |
|---|---|
| `load(TICKER, RANGE)` | Fetch price history, set the active ticker |
| `display()` | Render a chart for the active ticker |
| `compare(TICKER, RANGE)` | Render a second chart alongside the first |
| `range(RANGE)` | Change the time range for everything after it |
| `watch(TICKER, SECONDS, MINUTES)` | Live-refreshing chart on a timer |
| `unwatch()` | Stop the live chart |
| `monte(TICKER+TICKER, options...)` | Monte Carlo VaR/CVaR simulation |

## Getting started

### Requirements

- [Qt 6](https://www.qt.io/download) with the MinGW toolchain
- Python 3 with `yfinance`, `matplotlib`, `pandas`, `numpy`, and `scipy`
  installed (`pip install yfinance matplotlib pandas numpy scipy`)

### Build

`build.ps1` expects Qt installed at `C:\Qt\6.11.1\mingw_64` and the MinGW
compiler at `C:\Qt\Tools\mingw1310_64` - adjust the paths at the top of
the script if your install differs.

```powershell
.\build.ps1
```

This regenerates the Qt moc files, compiles everything, and launches the
app against `test_script.qtm`.

### Project layout

```
src/            C++ source (Qt widgets, syntax highlighter, command parser)
scripts/        Python scripts the app shells out to (yfinance, matplotlib)
resources/      Logo, button icons, and generated chart images (gitignored)
docs/           Language reference and notes
test_script.qtm Example script used by build.ps1 on launch
```

## Architecture

The C++ side is a thin shell: a `QPlainTextEdit`-based code editor with a
custom gutter (`CodeEditor`), a syntax highlighter for both the editor and
the console (`SyntaxHighlighter`), and a `CommandHandler` that owns all
script logic. Adding a new command means adding one case in
`CommandHandler::execute()` and a `handleX()` method - `MainWindow.cpp`
never needs to change.

Each command shells out to a dedicated Python script
(`scripts/fetchPrice.py`, `scripts/watchPrice.py`,
`scripts/montecarlo.py`), which does the actual data fetching and
plotting, then writes a PNG that the C++ side loads into a `QLabel`.
`watch()` runs its script asynchronously via a persistent `QProcess` so
the UI never blocks waiting on a live refresh.
