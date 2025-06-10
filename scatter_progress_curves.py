#!/usr/bin/env python3
"""
scatter_progress_curves.py
Generate one scatter plot per algorithm:
   x-axis = nodes expanded
   y-axis = elapsed time (s)
"""

import glob, re, pathlib
import pandas as pd, matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# Find all unique trace files
trace_files = {
    re.sub(r'^.*/trace_(.*)\.csv$', r'\1', str(p)):
    str(p)
    for p in pathlib.Path('.').rglob('trace_*.csv')
}

if not trace_files:
    print("⚠️  No trace_*.csv files found – aborting")
    exit()

for label, path in sorted(trace_files.items()):
    tr = pd.read_csv(path, names=["ms","nodes"])
    if tr.empty:
        print(f"  • {label}: empty trace, skipped")
        continue

    # swap axes: X = nodes, Y = time (s)
    plt.figure(figsize=(7,4))
    plt.scatter(tr.nodes, tr.ms / 1000.0, s=8)   # s= marker size
    plt.xlabel("Number of nodes")
    plt.ylabel("Time (s)")
    plt.title(f"Progress scatter: {label}")
    plt.grid(True)
    plt.tight_layout()
    outname = f"scatter_{label}.png"
    plt.savefig(outname, dpi=300)
    print(f"✅  {outname}")

print("\nAll scatter plots generated.")
