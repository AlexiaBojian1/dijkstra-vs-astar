//Lazy binary-heap Dijkstra: pushes every tentative edge cost into a binary heap and simply ignores outdated entries when popped, 
//trading extra inserts for simpler code.
#include "../helpers/timer.h"
#include "../helpers/graph_io.h"
#include <iostream>
#include <bits/stdc++.h>
#include <fstream>

using namespace std;
const double INF = numeric_limits<double>::infinity();

vector<double> dijkstra_lazy(const Graph &G, int s, int t, std::vector<std::pair<int, int>>& explored_edges, vector<int>& prev, Timer* timer)
{
    timer->start();
    int n = G.size();
    vector<double> dist(n, INF);
    vector<char> vis(n, 0);
    using Node = pair<double, int>;
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    dist[s] = 0;
    pq.emplace(0, s);

    // For path reconstruction
    timer->pause();
    prev.assign(n, -1);
    timer->start();

    while (!pq.empty())
    {
        auto [d, u] = pq.top();
        pq.pop();
        if (vis[u])
            continue;
        vis[u] = 1;
        if (u == t)
            break;
        for (auto [v, w] : G[u])
        {
            if (!vis[v] && d + w < dist[v])
            {
                dist[v] = d + w;

                timer->pause();
                prev[v] = u;
                timer->start();

                pq.emplace(dist[v], v);
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

int main()
{
    vector<pair<string, int>> datasets = {
        {"large", 0},
        {"Netherlands", 60000}
    };

    for (const auto& [name, source] : datasets) {

        string input = "../input_edges/graph_" + name + "_edges.txt";
        string explored_output = "../map_data/graph_" + name + "_visited_edges_dijk_lazy.txt";
        string path_output = "../map_data/graph_" + name + "_final_nodes_dijk_lazy.txt";

        Timer runtime;
        Graph G = read_graph(input);
        int s = source, t = G.size() - 1;
        vector<int> prev;
        std::vector<std::pair<int, int>> explored;

        auto dist = dijkstra_lazy(G, s, t, explored, prev, &runtime);
        write_edges(explored_output, explored);
        write_path(path_output, reconstruct_path(prev, t));

        cout << "Shortest distance (" << name << "): " << dist[t] << "\n";
        cout << "Time (" << name << "): " << runtime.elapsed() << " seconds\n"; 
    } 
}