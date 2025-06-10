import json
from pathlib import Path
from math import radians, sin, cos, sqrt, atan2

def load_nodes(dataset):
    nodes = {}
    nodes_file = f"../map_data/graph_{dataset}_nodes.txt"
    with open(nodes_file, 'r') as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) == 3:
                node_id, lat, lon = parts
                nodes[int(node_id)] = {
                    'lat': float(lat),
                    'lon': float(lon)
                }
    return nodes

def haversine(lat1, lon1, lat2, lon2):
    """Calculate distance between two points in kilometers"""
    R = 6371  # Earth radius in km
    lat1, lon1, lat2, lon2 = map(radians, [lat1, lon1, lat2, lon2])
    dlat = lat2 - lat1
    dlon = lon2 - lon1
    a = sin(dlat/2)**2 + cos(lat1) * cos(lat2) * sin(dlon/2)**2
    c = 2 * atan2(sqrt(a), sqrt(1-a))
    return R * c

def generate_geojson(dataset, algorithms, output_dir="output_geojson"):
    Path(output_dir).mkdir(parents=True, exist_ok=True)
    nodes = load_nodes(dataset)
    
    for algorithm in algorithms:
        print(f"Processing {dataset} with {algorithm}...")
        
        geojson = {
            "type": "FeatureCollection",
            "features": []
        }
        
        # File paths
        edges_file = f"../input_edges/graph_{dataset}_edges.txt"
        visited_file = f"../map_data/graph_{dataset}_visited_edges_{algorithm}.txt"
        final_file = f"../map_data/graph_{dataset}_final_nodes_{algorithm}.txt"
        
        # Load final path nodes
        final_nodes = set()
        if Path(final_file).exists():
            with open(final_file, 'r') as f:
                final_nodes = {int(line.strip()) for line in f}
        
        # Track all processed edges to avoid duplicates
        processed_edges = set()
        
        # 1. Process FINAL edges (red)
        final_edges = set()
        if Path(edges_file).exists() and final_nodes:
            with open(edges_file, 'r') as f:
                next(f)  # Skip header
                for line in f:
                    u, v, _ = map(int, line.strip().split())
                    if u in final_nodes and v in final_nodes:
                        final_edges.add((u, v))
                        processed_edges.add((u, v))
                        if u in nodes and v in nodes:
                            feature = create_edge_feature(nodes, u, v, algorithm, "final")
                            geojson["features"].append(feature)
        
        # 2. Process VISITED edges (blue)
        if Path(visited_file).exists():
            with open(visited_file, 'r') as f:
                for line in f:
                    u, v = map(int, line.strip().split())
                    if (u, v) not in processed_edges:
                        processed_edges.add((u, v))
                        if u in nodes and v in nodes:
                            edge_type = "visited"
                            feature = create_edge_feature(nodes, u, v, algorithm, edge_type)
                            geojson["features"].append(feature)
        
        # 3. Process ALL OTHER edges (black)
        if Path(edges_file).exists():
            with open(edges_file, 'r') as f:
                next(f)  # Skip header
                for line in f:
                    u, v, _ = map(int, line.strip().split())
                    if (u, v) not in processed_edges:
                        if u in nodes and v in nodes:
                            feature = create_edge_feature(nodes, u, v, algorithm, "all")
                            geojson["features"].append(feature)
        
        output_file = f"{output_dir}/{dataset}_{algorithm}_edges.geojson"
        with open(output_file, 'w') as f:
            json.dump(geojson, f, indent=2)
        
        print(f"Saved {len(geojson['features'])} edges to {output_file}")

def create_edge_feature(nodes, u, v, algorithm, edge_type):
    """Helper to create a GeoJSON feature with consistent properties"""
    return {
        "type": "Feature",
        "geometry": {
            "type": "LineString",
            "coordinates": [
                [nodes[u]['lon'], nodes[u]['lat']],
                [nodes[v]['lon'], nodes[v]['lat']]
            ]
        },
        "properties": {
            "algorithm": algorithm,
            "type": edge_type,
            "u": u,
            "v": v,
            "u_lat": nodes[u]['lat'],
            "u_lon": nodes[u]['lon'],
            "v_lat": nodes[v]['lat'],
            "v_lon": nodes[v]['lon'],
            "length_km": haversine(
                nodes[u]['lat'], nodes[u]['lon'],
                nodes[v]['lat'], nodes[v]['lon']
            )
        }
    }

if __name__ == "__main__":
    datasets = ["large"]
    algorithms = ["dijk_lazy", "dijk_decKey", "dijk_Fib", "astar_alt", "astar_weighted"]
    
    for dataset in datasets:
        generate_geojson(dataset, algorithms)