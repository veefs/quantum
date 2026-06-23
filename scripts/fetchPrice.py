import sys
import yfinance as yf

if len(sys.argv) < 2:
    print("Usage: python fetchPrice.py <TICKER>")
    sys.exit(1)

ticker_symbol = sys.argv[1]

ticker = yf.Ticker(ticker_symbol)
data = ticker.history(period="2y")

print(data.head())
print(data.shape)