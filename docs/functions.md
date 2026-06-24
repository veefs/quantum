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
