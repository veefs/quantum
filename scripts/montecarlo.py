import sys
import os
import datetime as dt
import numpy as np
import pandas as pd
import yfinance as yf
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from scipy.stats import norm, t

# monte() - Monte Carlo VaR/CVaR simulation for a portfolio of tickers.
# Usage: python montecarlo.py TICKER1,TICKER2,... [key=value key=value ...]
#
# Supported options (all optional):
#   days=100          simulation horizon in trading days
#   sims=400          number of Monte Carlo paths
#   investment=10000  starting portfolio value
#   alpha=5           confidence level for VaR/CVaR, in percent (5 = 95% CI)
#   dof=6             degrees of freedom for the t-distribution model
#   lookback=800      days of historical data to fetch for return stats
#   seed=42           random seed, for reproducible simulations
#   weights=0.6,0.4   manual portfolio weights, comma-separated, must
#                     match ticker count and need not sum to 1 (normalized)

if len(sys.argv) < 2:
    print("Usage: python montecarlo.py TICKER1,TICKER2 [key=value ...]")
    sys.exit(1)

tickers = [t.strip() for t in sys.argv[1].split(",") if t.strip()]

if len(tickers) < 2:
    print("Need at least 2 tickers for a portfolio.")
    sys.exit(1)

options = {}
for raw_opt in sys.argv[2:]:
    if "=" in raw_opt:
        key, value = raw_opt.split("=", 1)
        options[key.strip().lower()] = value.strip()

time_horizon = int(options.get("days", 100))
mc_sims = int(options.get("sims", 400))
initial_investment = float(options.get("investment", 10000))
alpha = float(options.get("alpha", 5))
dof = int(options.get("dof", 6))
lookback_days = int(options.get("lookback", 800))
seed = options.get("seed")
manual_weights = options.get("weights")

if seed is not None:
    np.random.seed(int(seed))

def get_data(stocks, start, end):
    stock_data = yf.download(stocks, start=start, end=end, progress=False)
    stock_data = stock_data["Close"]
    returns = stock_data.pct_change()
    mean_returns = returns.mean()
    cov_matrix = returns.cov()
    return returns, mean_returns, cov_matrix


def portfolio_performance(weights, mean_returns, cov_matrix, time):
    returns = np.sum(mean_returns * weights) * time
    std = np.sqrt(np.dot(weights.T, np.dot(cov_matrix, weights))) * np.sqrt(time)
    return returns, std


def mc_var(returns_series, alpha=5):
    if isinstance(returns_series, pd.Series):
        return np.percentile(returns_series, alpha)
    raise TypeError("Expected a pandas Series.")


def mc_cvar(returns_series, alpha=5):
    if isinstance(returns_series, pd.Series):
        below_var = returns_series <= mc_var(returns_series, alpha=alpha)
        return returns_series[below_var].mean()
    raise TypeError("Expected a pandas Series.")


end_date = dt.datetime.now()
start_date = end_date - dt.timedelta(days=lookback_days)

returns, mean_returns, cov_matrix = get_data(tickers, start_date, end_date)
returns = returns.dropna()

if returns.empty or len(returns) < 2:
    print("Not enough data returned for the given tickers/date range.")
    sys.exit(1)

if manual_weights:
    raw_weights = np.array([float(w) for w in manual_weights.split(",")])
    if len(raw_weights) != len(tickers):
        print(f"weights= has {len(raw_weights)} values but there are {len(tickers)} tickers.")
        sys.exit(1)
    weights = raw_weights / np.sum(raw_weights)
else:
    weights = np.random.random(len(tickers))
    weights /= np.sum(weights)

returns["portfolio"] = returns.dot(weights)

mean_m = np.full(shape=(time_horizon, len(weights)), fill_value=mean_returns)
mean_m = mean_m.T

portfolio_sims = np.full(shape=(time_horizon, mc_sims), fill_value=0.0)

for m in range(mc_sims):
    z = np.random.normal(size=(time_horizon, len(weights)))
    l = np.linalg.cholesky(cov_matrix)
    daily_returns = mean_m + np.inner(l, z)
    portfolio_sims[:, m] = np.cumprod(np.inner(weights, daily_returns.T) + 1) * initial_investment

alpha_frac = alpha / 100

# Historical VaR/CVaR (no distribution assumption)
h_var = -mc_var(returns["portfolio"], alpha=alpha) * np.sqrt(time_horizon)
h_cvar = -mc_cvar(returns["portfolio"], alpha=alpha) * np.sqrt(time_horizon)

# Parametric VaR/CVaR (normal + t-distribution)
p_ret, p_std = portfolio_performance(weights, mean_returns, cov_matrix, time_horizon)

norm_var = norm.ppf(1 - alpha_frac) * p_std - p_ret
norm_cvar = (alpha_frac ** -1) * norm.pdf(norm.ppf(alpha_frac)) * p_std - p_ret

t_var = np.sqrt((dof - 2) / dof) * t.ppf(alpha_frac, dof) * p_std - p_ret
xanu = t.ppf(alpha_frac, dof)
t_cvar = -1 / alpha_frac * (1 - dof) ** (-1) * (dof - 2 + xanu ** 2) * t.pdf(xanu, dof) * p_std - p_ret

# Monte Carlo VaR/CVaR from the simulated portfolio outcomes
port_results = pd.Series(portfolio_sims[-1, :])
mc_var_dollar = initial_investment - mc_var(port_results, alpha=alpha)
mc_cvar_dollar = initial_investment - mc_cvar(port_results, alpha=alpha)

ticker_label = ", ".join(tickers)
confidence_pct = round(100 - alpha, 1)

bg = "#282a36"
fg = "#f8f8f2"
grid_color = "#44475a"
line_color = "#8be9fd"
hist_color = "#50fa7b"

fig, ax = plt.subplots(figsize=(10, 6))
fig.patch.set_facecolor(bg)
ax.set_facecolor(bg)

for m in range(mc_sims):
    ax.plot(portfolio_sims[:, m], color=line_color, linewidth=0.4, alpha=0.25)

ax.axhline(initial_investment, color=hist_color, linewidth=1, linestyle="--")

ax.set_title(f"Monte Carlo: {ticker_label}  ({mc_sims} sims, {time_horizon}d)", color=fg, fontsize=12, loc="left")
ax.set_xlabel("Days", color=fg)
ax.set_ylabel("Portfolio Value ($)", color=fg)
ax.tick_params(colors=fg)
ax.grid(True, color=grid_color, alpha=0.4)

for spine in ax.spines.values():
    spine.set_color(grid_color)

plt.tight_layout()

os.makedirs("resources", exist_ok=True)
safe_label = "_".join(tickers)
out_path = os.path.join("resources", f"{safe_label}_monte.png")
plt.savefig(out_path, dpi=150, facecolor=bg)

print(f"Portfolio: {ticker_label}")
print(f"Weights:   {dict(zip(tickers, [round(w, 3) for w in weights]))}")
print()
print(f"VaR ({confidence_pct}% CI):")
print(f"  Historical : ${round(initial_investment * h_var, 2)}")
print(f"  Normal     : ${round(initial_investment * norm_var, 2)}")
print(f"  t-dist     : ${round(initial_investment * t_var, 2)}")
print(f"  Monte Carlo: ${round(mc_var_dollar, 2)}")
print()
print(f"CVaR ({confidence_pct}% CI):")
print(f"  Historical : ${round(initial_investment * h_cvar, 2)}")
print(f"  Normal     : ${round(initial_investment * norm_cvar, 2)}")
print(f"  t-dist     : ${round(initial_investment * t_cvar, 2)}")
print(f"  Monte Carlo: ${round(mc_cvar_dollar, 2)}")
