# Quantum Functions

One command per line. Arguments go in parentheses, separated by commas.

```
command(arg1, arg2)
```

---

## load(TICKER, RANGE)

Fetches a ticker's price history. RANGE is optional.

```
load(AMZN)
load(AMZN, 6mo)
```

## display()

Shows a chart for the ticker you loaded.

```
load(AMZN)
display()
```

## compare(TICKER, RANGE)

Shows a second chart next to the first one. RANGE is optional.

```
compare(AAPL)
```

## range(RANGE)

Changes the time range for everything after this line.

Valid ranges: `1d 5d 1mo 3mo 6mo 1y 2y 5y 10y ytd max`

```
range(6mo)
```

## watch(TICKER, SECONDS, MINUTES)

Live-updating chart. Refreshes automatically. All args optional.

```
watch(AMZN, 5, 30)
```

## unwatch()

Stops the live chart.

```
unwatch()
```

## monte(TICKER+TICKER, options...)

Runs a Monte Carlo simulation on a portfolio of 2+ tickers and prints
VaR/CVaR estimates (historical, normal, t-dist, and Monte Carlo) to the
console. Shows the simulation paths in the right chart panel. Tickers are
joined with `+`. Everything after that is optional `key=value` options,
in any order:

| Option       | Default | Meaning                                    |
|--------------|---------|---------------------------------------------|
| `days`       | 100     | simulation horizon, in trading days          |
| `sims`       | 400     | number of Monte Carlo paths                  |
| `investment` | 10000   | starting portfolio value                     |
| `alpha`      | 5       | confidence level for VaR/CVaR, in percent (5 = 95% CI) |
| `dof`        | 6       | degrees of freedom for the t-distribution model |
| `lookback`   | 800     | days of historical data used for return stats |
| `seed`       | random  | random seed, for reproducible runs           |
| `weights`    | random  | manual portfolio weights, comma-separated, one per ticker (need not sum to 1) |

```
monte(AMZN+AAPL)
monte(AMZN+AAPL+MSFT, days=250, sims=1000, alpha=1, seed=42)
monte(AMZN+AAPL, weights=0.7, 0.3)
```

---

## Colors

| Function   | Color  |
|------------|--------|
| load       | cyan   |
| display    | pink   |
| compare    | green  |
| range      | red    |
| watch      | orange |
| unwatch    | orange |
| monte      | purple |
