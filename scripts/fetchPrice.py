import sys
import yfinance as yf
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print("Usage: python fetchPrice.py <TICKER> [-d]")
    sys.exit(1)

ticker_symbol = sys.argv[1]
display_option = sys.argv[2] if len(sys.argv) > 2 else None

ticker = yf.Ticker(ticker_symbol)
data = ticker.history(period="2y")

print(data.head())
print(data.shape)

if display_option == "-d":
    plt.figure(figsize=(12, 6))
    plt.plot(data.index, data["Close"], label="Close Price")
    plt.title(f"{ticker_symbol} - 2 Year Price History")
    plt.xlabel("Date")
    plt.ylabel("Price (USD)")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    out_path = f"{ticker_symbol}_history.png"
    plt.savefig(out_path, dpi=150)

