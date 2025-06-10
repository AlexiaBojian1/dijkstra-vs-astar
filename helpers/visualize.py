import folium
import math

datasets = ["large", "Netherlands"]
datasets = ["Netherlands"]
algorithms = ["dijk_lazy", "dijk_decKey", "dijk_Fib", "astar_alt", "astar_weighted"]
algorithms = ["astar_alt"]

for dataset in datasets: 
    for algorithm in algorithms:

        print("==============   Dataset " + dataset + " and algorithm " + algorithm + "   =================")

        OUTPUT_MAP = "../output_maps/graph_" + dataset + "_map_" + algorithm + "_only_visited.html"
        NODES_TXT = "../map_data/graph_" + dataset + "_nodes.txt"

        # needs to be set to true if wants to be executed
        visualize_all = False
        EDGES_TXT = "../input_edges/graph_" + dataset + "_edges.txt"

        visualize_visited = True
        VISITED_TXT = "../map_data/graph_" + dataset + "_visited_edges_" + algorithm + ".txt"

        visualize_final = False
        FINAL_TXT = "../map_data/graph_" + dataset + "_final_nodes_" + algorithm + ".txt"


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

        # Compute bounding box of all coordinates
        all_coords = list(nodes.values())
        min_lat = min(lat for lat, lon in all_coords)
        max_lat = max(lat for lat, lon in all_coords)
        min_lon = min(lon for lat, lon in all_coords)
        max_lon = max(lon for lat, lon in all_coords)

        # Estimate zoom level from geographic extent
        lat_diff = max_lat - min_lat
        lon_diff = max_lon - min_lon
        max_diff = max(lat_diff, lon_diff)

        ## 51.444787 5.4805264
        ## 51.4304703 5.4520663



        def estimate_zoom(diff):
            thresholds = [
                (0.005, 16),
                (0.01, 15),
                (0.02, 14),
                (0.05, 13),
                (0.1, 12),
                (0.2, 11),
                (0.5, 10),
                (1.0, 9),
                (2.0, 8),
                (5.0, 7),
            ]
            for threshold, zoom in thresholds:
                if diff < threshold:
                    return zoom
            return 6  # fallback for very large areas

        zoom_level = estimate_zoom(max_diff)

        # Create map centered at middle point
        center_lat = (min_lat + max_lat) / 2
        center_lon = (min_lon + max_lon) / 2
        m = folium.Map(location=[center_lat, center_lon], zoom_start=zoom_level)


        final_path_edges = set()
        if visualize_final:
            print("Visualizing final path...")
            with open(FINAL_TXT, "r") as f:
                final_path = [int(line.strip()) for line in f]

            for i in range(len(final_path) - 1):
                u, v = final_path[i], final_path[i + 1]
                final_path_edges.add((u, v))
                coord1 = nodes[u]
                coord2 = nodes[v]
                folium.PolyLine([coord1, coord2], color='red', weight=4).add_to(m)

        visited_edges = set()
        if visualize_visited:
            print("Visualizing visited edges...")
            with open(VISITED_TXT, "r") as f:
                for line in f:
                    u, v = map(int, line.strip().split())
                    # Skip edges already in final path
                    if (u, v) in final_path_edges:
                        continue
                    visited_edges.add((u, v))
                    coord1 = nodes[u]
                    coord2 = nodes[v]
                    folium.PolyLine([coord1, coord2], color='blue', weight=2).add_to(m)

        if visualize_all:
            print("Visualizing all other edges...")
            with open(EDGES_TXT, "r") as f:
                header = f.readline()  # skip "num_nodes num_edges"
                for line in f:
                    u, v, _ = map(int, line.strip().split())
                    if (u, v) in final_path_edges or (u, v) in visited_edges:
                        continue
                    coord1 = nodes[u]
                    coord2 = nodes[v]
                    folium.PolyLine([coord1, coord2], color='black', weight=1).add_to(m)

        print("Saving...")
        m.save(OUTPUT_MAP)

print("All Done.")
