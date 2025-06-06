# python -m pip install overpy
# python generate_data.py
# https://algo.win.tue.nl/tutorials/openstreetmap/ (information about available APIs)

import overpy # we can load data from OSM using Overpass API, use roads as edges and junctions as nodes
import geopy.distance # we can closely approximate distance using geodesic distance between two points 


AREA_NAME = "Eindhoven"   
EDGES_TXT = "graph_large_edges.txt"
NODES_TXT = "graph_large_nodes.txt"


api = overpy.Overpass()
query = f"""
[out:json][timeout:60];
area["name"="{AREA_NAME}"]->.searchArea;
(
  way["highway"](area.searchArea);
  >;
);
out body;
"""

print(f"Querying Overpass API...")
result = api.query(query) 

# has result.nodes and result.ways
# each node has .id, .lat, .lon, .tags
# example: print(f"Node {node.id}: ({node.lat}, {node.lon})") gives Node 5686022: (51.4447870, 5.4805264)
# each way has .id, .nodes, .tags
# example: print(f"Way {way.id}: {[n.id for n in way.nodes]}") gives Way 4243215: [652808599, 1931472267, 1931472269, 1430537649, 1520517695, 25243566]
# print(f"Tags: {way.tags}") gives Tags: {'bicycle': 'permissive', 'highway': 'pedestrian', 'lit': 'yes', 'name': 'Groot Paradijs', 'surface': 'paving_stones', 'zone:traffic': 'NL:urban'}

print("Filling in nodes dictionary with OSM values...")
nodes = {node.id: (float(node.lat), float(node.lon)) for node in result.nodes}

print("Filling in edges array with OSM values FromID, ToID, Length...")
edges = []
for way in result.ways:
    is_oneway = way.tags.get("oneway") == "yes"
    node_ids = [n.id for n in way.nodes]

    # forward direction
    for i in range(len(node_ids) - 1):
        from_id, to_id = node_ids[i], node_ids[i + 1]
        coord1, coord2 = nodes[from_id], nodes[to_id]
        dist = geopy.distance.geodesic(coord1, coord2).meters
        edges.append((from_id, to_id, round(dist)))

    # backward direction if not oneway
    if not is_oneway:
        for i in range(len(node_ids) - 1):
            from_id, to_id = node_ids[i + 1], node_ids[i]
            coord1, coord2 = nodes[from_id], nodes[to_id]
            dist = geopy.distance.geodesic(coord1, coord2).meters
            edges.append((from_id, to_id, round(dist)))

print("Mapping OSM node IDs to 0-based and consecutive IDs...")
nodes_reindexed = {node_id: id for id, node_id in enumerate(nodes)}

print("Write number of nodes, edges, and for each edge reindexed FromID ToID Length into the input file...")
num_nodes = len(nodes_reindexed)
num_edges = len(edges)

with open(EDGES_TXT, "w") as f:
    f.write(f"{num_nodes} {num_edges}\n")
    for from_osm, to_osm, length in edges:
        from_id = nodes_reindexed[from_osm]
        to_id = nodes_reindexed[to_osm]
        f.write(f"{from_id} {to_id} {length}\n")

print("Write IDs and corresponding lat and lon for each node into a file for future visualization...")
with open(NODES_TXT, "w") as f:
    for osm_id, (lat, lon) in nodes.items():
        f.write(f"{nodes_reindexed[osm_id]} {lat} {lon}\n")

print("Done.")
