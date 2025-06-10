#!/usr/bin/env python3
import glob, os, re
import pandas as pd
import matplotlib.pyplot as plt

# ----------  A. read aggregate CSV  ----------
df = pd.read_csv("benchmarks.csv")
df["Time_s"] = df["Time_ms"] / 1000.0
df["Mem_MB"] = df["Mem_KB"] / 1024.0

# ---------- 1. Speed-up bar vs djik_lazy -----
baseline = df.loc[df.Algorithm == "dijk_lazy", "Time_s"].iloc[0]
df["Speedup"] = baseline / df["Time_s"]

plt.figure(figsize=(6,4))
plt.bar(df.Algorithm, df.Speedup)
plt.ylabel("Speed-up (× vs djik_lazy)")
plt.title("Relative runtime")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.savefig("speedup_bar.png", dpi=300)

# ---------- 2. Pareto scatter: memory vs time -
plt.figure(figsize=(6,4))
plt.scatter(df.Mem_MB, df.Time_s)
for _, r in df.iterrows():
    plt.annotate(r.Algorithm, (r.Mem_MB, r.Time_s),
                 xytext=(5,5), textcoords="offset points", fontsize=8)
plt.xlabel("Peak RSS (MB)")
plt.ylabel("Time (s)")
plt.title("Pareto frontier: memory vs time")
plt.grid(True)
plt.tight_layout()
plt.savefig("time_vs_mem.png", dpi=300)

# ---------- 3. Progress curves ---------------
plt.figure(figsize=(6,4))
trace_files = sorted(glob.glob("**/trace_*.csv", recursive=True))
palette = plt.cm.get_cmap("tab10", len(trace_files))  # colour cycle

for idx, path in enumerate(trace_files):
    tr = pd.read_csv(path, names=["ms","nodes"])
    lbl = re.sub(r'^.*/trace_(.*)\.csv$', r'\1', path)  # strip folder + ext
    plt.plot(tr.ms/1000.0, tr.nodes,
             label=lbl, color=palette(idx), linewidth=1)

plt.xlabel("Elapsed time (s)")
plt.ylabel("Nodes discovered")
plt.title("Search progress on large graph")
plt.legend(fontsize=7)
plt.tight_layout()
plt.savefig("progress_curves.png", dpi=300)

print("✅  speedup_bar.png  time_vs_mem.png  progress_curves.png  written")
