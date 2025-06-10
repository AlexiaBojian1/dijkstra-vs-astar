# python -m pip install overpy
# python generate_data.py
# https://algo.win.tue.nl/tutorials/openstreetmap/ (information about available APIs)

import overpy # we can load data from OSM using Overpass API, use roads as edges and junctions as nodes
import geopy.distance # we can closely approximate distance using geodesic distance between two points 


AREA_NAME = "Netherlands"   
##area["name"="{AREA_NAME}"]->.searchArea;
area_id = 3600047796  # Area ID for Netherlands
EDGES_TXT = "../input_edges/graph_" + AREA_NAME + "_edges.txt"
NODES_TXT = "../map_data/graph_" + AREA_NAME + "_nodes.txt"


api = overpy.Overpass()
def query_highways(highway_type):
    print(f"Querying Overpass API for '{highway_type}' roads...")
    query = f"""
    [out:json][timeout:1200];
    area({area_id})->.searchArea;
    (
      way["highway"="{highway_type}"](area.searchArea);
    );
    out body;
    >;
    out skel qt;
    """
    return api.query(query)

# Query separately
result_primary = query_highways("primary")
result_secondary = query_highways("secondary")


# has result.nodes and result.ways
# each node has .id, .lat, .lon, .tags
# example: print(f"Node {node.id}: ({node.lat}, {node.lon})") gives Node 5686022: (51.4447870, 5.4805264)
# each way has .id, .nodes, .tags
# example: print(f"Way {way.id}: {[n.id for n in way.nodes]}") gives Way 4243215: [652808599, 1931472267, 1931472269, 1430537649, 1520517695, 25243566]
# print(f"Tags: {way.tags}") gives Tags: {'bicycle': 'permissive', 'highway': 'pedestrian', 'lit': 'yes', 'name': 'Groot Paradijs', 'surface': 'paving_stones', 'zone:traffic': 'NL:urban'}

# Merge nodes
print("Merging node data...")
all_nodes = {node.id: (float(node.lat), float(node.lon)) for node in result_primary.nodes}
for node in result_secondary.nodes:
    all_nodes[node.id] = (float(node.lat), float(node.lon))  # override or add

# Merge ways
all_ways = result_primary.ways + result_secondary.ways

print("Filling in edges array with OSM values FromID, ToID, Length...")
edges = []
for way in all_ways:
    is_oneway = way.tags.get("oneway") == "yes"
    node_ids = [n.id for n in way.nodes]

    # forward direction
    for i in range(len(node_ids) - 1):
        from_id, to_id = node_ids[i], node_ids[i + 1]
        coord1, coord2 = all_nodes[from_id], all_nodes[to_id]
        dist = geopy.distance.geodesic(coord1, coord2).meters
        edges.append((from_id, to_id, round(dist)))

    # backward direction if not oneway
    if not is_oneway:
        for i in range(len(node_ids) - 1):
            from_id, to_id = node_ids[i + 1], node_ids[i]
            coord1, coord2 = all_nodes[from_id], all_nodes[to_id]
            dist = geopy.distance.geodesic(coord1, coord2).meters
            edges.append((from_id, to_id, round(dist)))

print("Mapping OSM node IDs to 0-based and consecutive IDs...")
nodes_reindexed = {node_id: id for id, node_id in enumerate(all_nodes)}

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
    for osm_id, (lat, lon) in all_nodes.items():
        f.write(f"{nodes_reindexed[osm_id]} {lat} {lon}\n")

print("Done.")
