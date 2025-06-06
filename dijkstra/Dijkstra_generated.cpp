//g++ -std=c++17 Dijkstra_generated.cpp -o dijkstra.exe
//./dijkstra.exe

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <limits>
#include <unordered_set>
#include <stack>

using namespace std;

const int INF = numeric_limits<int>::max();

// Structure for edges
struct Edge {
    int to;
    int weight;
};

int main() {
    // --- Parameters ---
    const string input_file = "../map_data/graph_large_edges.txt";
    const string explored_output = "../map_data/graph_large_visited_edges.txt";
    const string path_output = "../map_data/graph_large_final_nodes.txt";
    const int start_node = 100;     // <---- Hardcoded start
    const int target_node = 250;  // <---- Hardcoded target (adjust!)

    // --- Read graph ---
    ifstream infile(input_file);
    if (!infile) {
        cerr << "Failed to open graph.txt\n";
        return 1;
    }

    int n, m;
    infile >> n >> m;

    vector<vector<Edge>> adj(n);
    for (int i = 0; i < m; ++i) {
        int u, v, w;
        infile >> u >> v >> w;
        adj[u].push_back({v, w});
    }

    // --- Dijkstra ---
    vector<int> dist(n, INF);
    vector<int> prev(n, -1);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
    unordered_set<string> explored_edges;

    dist[start_node] = 0;
    pq.push({0, start_node});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;

        for (const auto& edge : adj[u]) {
            int v = edge.to, w = edge.weight;
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push({dist[v], v});
            }
            // Log all explored (visited) edges
            explored_edges.insert(to_string(u) + " " + to_string(v));
        }
    }

    // --- Write explored edges ---
    ofstream explored_out(explored_output);
    for (const auto& entry : explored_edges) {
        explored_out << entry << "\n";
    }

    // --- Reconstruct final path ---
    ofstream path_out(path_output);
    stack<int> path;
    int current = target_node;
    while (current != -1) {
        path.push(current);
        current = prev[current];
    }

    if (dist[target_node] == INF) {
        cerr << "No path found from " << start_node << " to " << target_node << "\n";
    } else {
        int u = path.top(); path.pop();
        while (!path.empty()) {
            int v = path.top(); path.pop();
            path_out << u << " " << v << "\n";
            u = v;
        }
    }

    cout << "✅ Dijkstra complete.\n";
    cout << "🛣  Shortest distance: " << dist[target_node] << " meters\n";
    cout << "📄 Explored: " << explored_output << "\n";
    cout << "📄 Final path: " << path_output << "\n";

    return 0;
}
