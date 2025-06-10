#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("benchmarks.csv")
df["Time_s"] = df["Time_ms"] / 1000.0
df["Mem_MB"] = df["Mem_KB"] / 1024.0

# 1 Scatter: time vs nodes
plt.figure(figsize=(6,4))
plt.scatter(df["Nodes"], df["Time_s"])
for _, r in df.iterrows():
    plt.annotate(r.Algorithm, (r.Nodes, r.Time_s),
                 xytext=(5,5), textcoords="offset points", fontsize=8)
plt.xlabel("Nodes discovered"); plt.ylabel("Time (s)")
plt.title("Time vs Nodes (large graph)"); plt.grid(True); plt.tight_layout()
plt.savefig("time_vs_nodes.png", dpi=300)

# 2 Bar: runtime
plt.figure(figsize=(6,4))
plt.bar(df["Algorithm"], df["Time_s"])
plt.ylabel("Time (s)"); plt.xticks(rotation=45, ha="right")
plt.title("Average runtime (2 runs)"); plt.tight_layout()
plt.savefig("runtime_bar.png", dpi=300)

# 3 Summary table
fig, ax = plt.subplots(figsize=(6,2))
ax.axis("off")
tbl = ax.table(cellText=df[["Algorithm","Time_s","Nodes","Mem_MB"]].values,
               colLabels=["Algorithm","Time (s)","Nodes","Memory (MB)"],
               loc="center")
tbl.auto_set_font_size(False); tbl.set_fontsize(8); tbl.scale(1.2,1.2)
plt.tight_layout(); plt.savefig("summary_table.png", dpi=300)

print("✅  3 plot images generated")
