import folium

# always needs to be set
OUTPUT_MAP = "../outputmaps/graph_large_map.html"
NODES_TXT = "../map_data/graph_large_nodes.txt"

# needs to be set to true if wants to be executed
visualize_all = False
EDGES_TXT = "../input_edges/graph_large_edges.txt"

visualize_visited = True
VISITED_TXT = "../map_data/graph_large_visited_edges.txt"

visualize_final = True
FINAL_TXT = "../map_data/graph_large_final_nodes.txt"


print("Reading the nodes from file...")
nodes = {}
with open(NODES_TXT, "r") as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) != 3:
            continue
        node_id, lat, lon = parts
        nodes[int(node_id)] = (float(lat), float(lon))

print(f"Creating map...")
center = list(nodes.values())[0]
m = folium.Map(location=center, zoom_start=13)

if(visualize_all):
    print("Visualizing all edges...")
    edges = []
    with open(EDGES_TXT, "r") as f:
        header = f.readline()  # skip "num_nodes num_edges"
        for line in f:
            from_id, to_id, length = line.strip().split()
            edges.append((int(from_id), int(to_id)))

    for from_id, to_id in edges:
        coord1 = nodes[from_id]
        coord2 = nodes[to_id]
        folium.PolyLine([coord1, coord2], color='black', weight=1).add_to(m)

if(visualize_visited):
    print("Visualizing visited edges...")
    visited_edges = []
    with open(VISITED_TXT, "r") as f:
        for line in f:
            from_id, to_id = line.strip().split()
            visited_edges.append((int(from_id), int(to_id)))
    
    for from_id, to_id in visited_edges:
        coord1 = nodes[from_id]
        coord2 = nodes[to_id]
        folium.PolyLine([coord1, coord2], color='blue', weight=2).add_to(m)

if(visualize_final):
    print("Visualizing final path...")
    with open(FINAL_TXT, "r") as f:
        final_path = [int(line.strip()) for line in f]

    for i in range(len(final_path) - 1):
        coord1 = nodes[final_path[i]]
        coord2 = nodes[final_path[i + 1]]
        folium.PolyLine([coord1, coord2], color='red', weight=4).add_to(m)

print("Saving...")
m.save(OUTPUT_MAP)
print("Done.")
