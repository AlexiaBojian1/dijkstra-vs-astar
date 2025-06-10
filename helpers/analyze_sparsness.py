# analyze_sparsness.py


datasets = ["large", "Netherlands"]
dataset = datasets[1]
EDGE_FILE = "../input_edges/graph_" + dataset + "_edges.txt"


with open(EDGE_FILE, "r") as f:
    n, m = map(int, f.readline().split())
    out_degree = {}
    all_nodes = set()

    for line in f:
        u, v, _ = map(int, line.strip().split())
        out_degree[u] = out_degree.get(u, 0) + 1
        all_nodes.add(u)
        all_nodes.add(v)

# --- Sparsity Calculation ---
max_possible_edges = n * (n - 1)  # directed
sparsity = 1 - (m / max_possible_edges)

# --- Out-degree statistics ---
from collections import Counter
degree_counts = Counter(out_degree.values())

print(f"Nodes listed in file header: {n}")
print(f"Unique node IDs found in edges: {len(all_nodes)}")
print(f"Edges: {m}")
print(f"Sparsity: {sparsity:.6f} (1 = no edges, 0 = fully connected)")

print("\nOut-degree distribution:")
for degree in sorted(degree_counts):
    print(f"  {degree} outgoing edges → {degree_counts[degree]} nodes")

num_nodes_with_zero_out = n - len(out_degree)
print(f"\nNodes with 0 outgoing edges: {num_nodes_with_zero_out}")
