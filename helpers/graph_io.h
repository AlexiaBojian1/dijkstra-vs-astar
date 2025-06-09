#ifndef GRAPH_IO_H
#define GRAPH_IO_H

#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>
#include <unordered_set>

// Graph structure
struct Edge {
    int to;
    double w;
};
using Graph = std::vector<std::vector<Edge>>;

// Read graph from file
inline Graph read_graph(const std::string& filename) {
    std::ifstream in(filename);
    int n, m;
    in >> n >> m;
    Graph G(n);
    for (int i = 0, u, v; i < m; ++i) {
        double w;
        in >> u >> v >> w;
        G[u].push_back({v, w});
        G[v].push_back({u, w});  // Remove if graph is directed
    }
    return G;
}

// Reconstruct final path
inline std::vector<int> reconstruct_path(const std::vector<int>& prev, int target) {
    std::vector<int> path;
    for (int at = target; at != -1; at = prev[at])
        path.push_back(at);
    std::reverse(path.begin(), path.end());
    return path;
}

// Write final path into a file
inline void write_path(const std::string& filename, const std::vector<int>& path) {
    std::ofstream out(filename);
    for (int node : path)
        out << node << "\n";
}

// Write explored edges into a file
inline void write_edges(const std::string& filename, const std::vector<std::pair<int, int>>& edges) {
    std::ofstream out(filename);
    for (auto [u, v] : edges)
        out << u << " " << v << "\n";
}

#endif  // GRAPH_IO_H

    // const string explored_output = "../map_data/graph_large_visited_edges.txt";
    // const string path_output = "../map_data/graph_large_final_nodes.txt";

