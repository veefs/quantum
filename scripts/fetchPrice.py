import sys
import os
import yfinance as yf
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print("Usage: python fetchPrice.py <TICKER> [TIME PERIOD] [INTERVAL] [-d]")
    sys.exit(1)

# -d is a flag and can appear anywhere, so pull it out first and treat
# everything else as positional: ticker, period, optional interval.
args = sys.argv[1:]
display_option = "-d" if "-d" in args else None
positional = [a for a in args if a != "-d"]

ticker_symbol = positional[0]
time_option = positional[1] if len(positional) > 1 else "2y"
interval_option = positional[2] if len(positional) > 2 else None

ticker = yf.Ticker(ticker_symbol)

if interval_option:
    data = ticker.history(period=time_option, interval=interval_option)
else:
    data = ticker.history(period=time_option)

print(data.head())
print(data.shape)

if display_option == "-d":
    plt.figure(figsize=(12, 6))
    plt.plot(data.index, data["Close"], label="Close Price")
    title_suffix = f"{time_option} / {interval_option}" if interval_option else time_option
    plt.title(f"{ticker_symbol} - {title_suffix} History")
    plt.xlabel("Date")
    plt.ylabel("Price (USD)")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    os.makedirs("resources", exist_ok=True)
    out_path = os.path.join("resources", f"{ticker_symbol}_{time_option}_history.png")
    plt.savefig(out_path, dpi=150)
