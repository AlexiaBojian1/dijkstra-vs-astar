#!/usr/bin/env python3
"""
plots_correct.py  –  generate four key figures *after* validating the data

Output PNGs:
  • time_vs_nodes.png
  • speedup_bar.png
  • time_vs_mem.png
  • progress_curves.png
"""

import sys, glob, re, textwrap, pandas as pd, matplotlib
matplotlib.use("Agg")          # headless backend
import matplotlib.pyplot as plt

###############################################################################
# 1. READ & SANITISE THE CSV
###############################################################################
def num(x):
    """strip thousands-separators, spaces; return int"""
    return int(re.sub(r"[,\s]", "", str(x)))

try:
    df = pd.read_csv(
        "benchmarks.csv",
        converters={"Time_ms": num, "Nodes": num, "Mem_KB": num},
    )
except FileNotFoundError:
    sys.exit("❌  benchmarks.csv not found – run ./run_benchmarks.sh first")
except ValueError as e:
    sys.exit(f"❌  Could not parse numbers in CSV: {e}")

# Clean algorithm labels
df["Algorithm"] = df["Algorithm"].str.strip()
df["AlgSimple"] = df["Algorithm"].str.replace(r".*/", "", regex=True)

# Optional peek
print("\nCleaned data:")
print(df.to_string(index=False))

# Basic sanity checks ---------------------------------------------------------
if df[["Time_ms", "Nodes", "Mem_KB"]].le(0).any().any():
    sys.exit("❌  At least one metric is ≤ 0 – check your CSV values!")

if df.AlgSimple.duplicated().any():
    dupes = df.loc[df.AlgSimple.duplicated(), "AlgSimple"].unique()
    sys.exit(f"❌  Duplicate algorithm rows in CSV: {', '.join(dupes)}")

###############################################################################
# 2. DERIVED COLUMNS
###############################################################################
df["Time_s"] = df.Time_ms / 1000.0
df["Mem_MB"] = df.Mem_KB / 1024.0

###############################################################################
# 3. PLOTS
###############################################################################
# ---------- 3.1 runtime vs nodes --------------------------------------------
plt.figure(figsize=(6,4))
plt.scatter(df.Nodes, df.Time_s)
for _, r in df.iterrows():
    plt.annotate(r.AlgSimple, (r.Nodes, r.Time_s),
                 xytext=(5,5), textcoords="offset points", fontsize=8)
plt.xlabel("Nodes expanded")
plt.ylabel("Time (s)")
plt.title("Runtime vs. search effort")
plt.grid(True)
plt.tight_layout()
plt.savefig("time_vs_nodes.png", dpi=300)

# ---------- 3.2 speed-up bar chart ------------------------------------------
baseline_row = df.loc[df.AlgSimple.eq("dijk_lazy")]
if baseline_row.empty:
    # Fallback: first Dijkstra row
    baseline_row = df[df.AlgSimple.str.contains("dijk")].head(1)
    if baseline_row.empty:
        sys.exit("❌  No baseline row called 'dijk_lazy' or any 'dijk_*' found")
baseline = baseline_row.Time_s.iat[0]

df["Speedup"] = baseline / df.Time_s
plt.figure(figsize=(6,4))
plt.bar(df.AlgSimple, df.Speedup)
plt.ylabel(f"Speed-up (× vs {baseline_row.AlgSimple.iat[0]})")
plt.title("Relative runtime")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.savefig("speedup_bar.png", dpi=300)

# ---------- 3.3 Pareto: memory vs time --------------------------------------
plt.figure(figsize=(6,4))
plt.scatter(df.Mem_MB, df.Time_s)
for _, r in df.iterrows():
    plt.annotate(r.AlgSimple, (r.Mem_MB, r.Time_s),
                 xytext=(5,5), textcoords="offset points", fontsize=8)
plt.xlabel("Peak RSS (MB)")
plt.ylabel("Time (s)")
plt.title("Pareto frontier: memory vs time")
plt.grid(True)
plt.tight_layout()
plt.savefig("time_vs_mem.png", dpi=300)

# ---------- 3.4 progress curves ---------------------------------------------
# ---------- 3.4 progress curves (fixed) -------------------------------
import pathlib
plt.figure(figsize=(6,4))

# ❶  build a dict label → file path   (deduplicates automatically)
trace_files = {
    re.sub(r'^.*/trace_(.*)\.csv$', r'\1', str(p)):
    str(p)
    for p in pathlib.Path('.').rglob('trace_*.csv')
}

if not trace_files:
    print("⚠️  No trace_*.csv files found – skipping progress_curves.png")
else:
    palette = plt.colormaps["tab10"].resampled(len(trace_files))
    colour_idx = 0
    for label, path in sorted(trace_files.items()):
        tr = pd.read_csv(path, names=['ms','nodes'])
        if tr.empty:
            print(f"  • {label}: trace is empty, skipped")
            continue

        # ❷  sanity-check against the aggregate CSV
        final_nodes = tr.nodes.iloc[-1]
        ref_row = df.loc[df.AlgSimple == label]
        if not ref_row.empty and abs(final_nodes - ref_row.Nodes.iat[0]) > 500:
            print(f"  • {label}: trace disagrees with CSV "
                  f"({final_nodes} vs {ref_row.Nodes.iat[0]}), skipped")
            continue   # don’t plot stale / wrong trace

        plt.plot(tr.ms/1000.0, tr.nodes,
                 label=label, linewidth=1,
                 color=palette(colour_idx))
        colour_idx += 1

    if colour_idx == 0:
        print("⚠️  All traces skipped – no progress plot generated")
    else:
        plt.xlabel("Elapsed time (s)")
        plt.ylabel("Nodes expanded")
        plt.title("Search progress on large graph")
        plt.legend(fontsize=7)
        plt.tight_layout()
        plt.savefig("progress_curves.png", dpi=300)
        print("   • progress_curves.png")

print("\n✅  Plots generated:\n"
      "   • time_vs_nodes.png\n"
      "   • speedup_bar.png\n"
      "   • time_vs_mem.png\n"
      "   • progress_curves.png (if traces present)")
