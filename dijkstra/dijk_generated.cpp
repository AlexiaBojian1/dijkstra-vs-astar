//g++ -std=c++17 dijk_generated.cpp -o dijkstra.exe
//./dijkstra.exe

#include "../helpers/timer.h"
#include "../helpers/graph_io.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <limits>
#include <unordered_set>
#include <stack>

using namespace std;
const double INF = numeric_limits<double>::infinity();

vector<double> run_dijk_generated(const Graph &G, int s, int t, std::vector<std::pair<int, int>>& explored_edges, vector<int>& prev, Timer* timer) {
    
    timer->start();
    int n = G.size();
    vector<double> dist(n, INF);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
    dist[s] = 0;
    pq.push({0, s});

    // For path reconstruction
    timer->pause();
    prev.assign(n, -1);
    timer->start();

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;

        for (const auto& edge : G[u]) {
            int v = edge.to;
            double w = edge.w;
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                
                timer->pause();
                prev[v] = u;
                timer->start();

                pq.push({dist[v], v});
            }

            // Logging visited edges
            timer->pause();
            explored_edges.push_back({u, v});
            timer->start();
        }
    }

    timer->pause();
    return dist;
}

int main() {

    string input = "../input_edges/graph_large_edges.txt";
    string explored_output = "../map_data/graph_large_visited_edges_dijk_generated.txt";
    string path_output = "../map_data/graph_large_final_nodes_dijk_generated.txt";

    Timer runtime;
    Graph G = read_graph(input);
    int s = 0, t = G.size() - 1;
    vector<int> prev;
    std::vector<std::pair<int, int>> explored;

    auto dist = run_dijk_generated(G, s, t, explored, prev, &runtime); 
    write_edges(explored_output, explored);
    write_path(path_output, reconstruct_path(prev, t));

    cout << "Shortest distance: " << dist[t] << "\n";
    cout << "Time: " << runtime.elapsed() << " seconds\n";
}
