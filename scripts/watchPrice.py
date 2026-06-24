import sys
import os
import yfinance as yf
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

# Dedicated script for watch() - purpose-built for a tight, live-feeling
# view rather than a full historical report. Always pulls 1-minute bars
# and only plots the most recent window of them.

if len(sys.argv) < 2:
    print("Usage: python watchPrice.py <TICKER> [WINDOW_MINUTES]")
    sys.exit(1)

ticker_symbol = sys.argv[1]
window_minutes = int(sys.argv[2]) if len(sys.argv) > 2 else 60

ticker = yf.Ticker(ticker_symbol)
data = ticker.history(period="1d", interval="1m")

print(data.tail())
print(data.shape)

if data.empty:
    sys.exit(0)

# Trim to the last N minutes so each refresh visibly moves.
data = data.tail(window_minutes)

last_price = data["Close"].iloc[-1]
last_time = data.index[-1]

bg = "#282a36"
fg = "#f8f8f2"
grid_color = "#44475a"
line_color = "#8be9fd"
dot_color = "#50fa7b"

fig, ax = plt.subplots(figsize=(8, 5))
fig.patch.set_facecolor(bg)
ax.set_facecolor(bg)

ax.plot(data.index, data["Close"], color=line_color, linewidth=1.5)
ax.scatter([last_time], [last_price], color=dot_color, s=40, zorder=5)
ax.annotate(
    f"{last_price:.2f}",
    xy=(last_time, last_price),
    xytext=(8, 0),
    textcoords="offset points",
    color=dot_color,
    fontsize=11,
    fontweight="bold",
    va="center",
)

ax.set_title(f"{ticker_symbol}  ·  live", color=fg, fontsize=13, loc="left")
ax.tick_params(colors=fg)
ax.xaxis.set_major_formatter(mdates.DateFormatter("%H:%M"))
ax.grid(True, color=grid_color, alpha=0.4)

for spine in ax.spines.values():
    spine.set_color(grid_color)

plt.tight_layout()

os.makedirs("resources", exist_ok=True)
out_path = os.path.join("resources", f"{ticker_symbol}_watch.png")
plt.savefig(out_path, dpi=150, facecolor=bg)
