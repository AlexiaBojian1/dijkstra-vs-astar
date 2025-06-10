#!/usr/bin/env python3
import glob, re, pandas as pd, matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# ---------- read & sanitise CSV ---------- #
def num(x): return int(str(x).replace(',', '').strip())
df = pd.read_csv("benchmarks.csv",
                 converters={"Time_ms": num, "Nodes": num, "Mem_KB": num})
df["Algorithm"] = df["Algorithm"].str.strip()
df["AlgSimple"] = df["Algorithm"].str.replace(r".*/", "", regex=True)
df["Time_s"]    = df["Time_ms"] / 1000
df["Mem_MB"]    = df["Mem_KB"] / 1024

# ---------- 1. scatter time vs nodes ----- #
plt.figure(figsize=(6,4))
plt.scatter(df.Nodes, df.Time_s)
for _, r in df.iterrows():
    plt.annotate(r.AlgSimple, (r.Nodes, r.Time_s),
                 xytext=(4,4), textcoords="offset points", fontsize=8)
plt.xlabel("Nodes expanded"); plt.ylabel("Time (s)")
plt.title("Runtime vs. search effort"); plt.grid(True)
plt.tight_layout(); plt.savefig("time_vs_nodes.png", dpi=300)

# ---------- 2. speed-up bar -------------- #
baseline = df.loc[df.AlgSimple == "dijk_lazy", "Time_s"].iloc[0]
df["Speedup"] = baseline / df.Time_s
plt.figure(figsize=(6,4))
plt.bar(df.AlgSimple, df.Speedup)
plt.ylabel("Speed-up (× vs dijk_lazy)")
plt.title("Relative runtime"); plt.xticks(rotation=45, ha="right")
plt.tight_layout(); plt.savefig("speedup_bar.png", dpi=300)

# ---------- 3. Pareto frontier ----------- #
plt.figure(figsize=(6,4))
plt.scatter(df.Mem_MB, df.Time_s)
for _, r in df.iterrows():
    plt.annotate(r.AlgSimple, (r.Mem_MB, r.Time_s),
                 xytext=(4,4), textcoords="offset points", fontsize=8)
plt.xlabel("Peak RSS (MB)"); plt.ylabel("Time (s)")
plt.title("Pareto frontier: memory vs. time"); plt.grid(True)
plt.tight_layout(); plt.savefig("time_vs_mem.png", dpi=300)

# ---------- 4. progress curves ----------- #
plt.figure(figsize=(6,4))
seen = set()
for path in sorted(glob.glob("**/trace_*.csv", recursive=True)):
    lbl = re.sub(r'^.*/trace_(.*)\.csv$', r'\1', path)
    if lbl in seen:     # avoid duplicates
        continue
    seen.add(lbl)
    tr = pd.read_csv(path, names=["ms","nodes"])
    plt.plot(tr.ms/1000.0, tr.nodes, label=lbl, linewidth=1)
plt.xlabel("Elapsed time (s)"); plt.ylabel("Nodes expanded")
plt.title("Search progress on large graph")
plt.legend(fontsize=7); plt.tight_layout()
plt.savefig("progress_curves.png", dpi=300)

print("✅  four plots regenerated with clean data.")
