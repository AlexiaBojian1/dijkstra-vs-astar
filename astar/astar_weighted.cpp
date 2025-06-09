//Weighted A* (ε-optimal): uses f = g + w·h with w > 1 (e.g., 1.5) to bias the search toward the goal, finding a path no worse than w times optimal much faster.

#include "../helpers/timer.h"
#include "../helpers/graph_io.h"
#include <iostream>
// Weighted A* (ε-optimal): uses f = g + w·h with w > 1 (e.g., 1.5) to bias the search toward the goal, finding a path no worse than w times optimal much faster.

#include <bits/stdc++.h>

using namespace std;
const double INF = numeric_limits<double>::infinity();

/* ---------- Weighted A* ---------- */
vector<double> astar_weighted(const Graph &G,
                              int s, int t,
                              const vector<double> &h,
                              double w,
                              std::vector<std::pair<int, int>>& explored_edges, 
                              vector<int>& prev, 
                              Timer* timer                            
                            )
{
    timer->start();
    int n = G.size();
    vector<double> g(n, INF);
    vector<char> vis(n, 0);

    using Node = pair<double, int>; // (f,vertex)
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    g[s] = 0;
    pq.emplace(w * h[s], s); // f = g + w·h

    // For path reconstruction
    timer->pause();
    prev.assign(n, -1);
    timer->start();


    while (!pq.empty())
    {
        auto [f, u] = pq.top();
        pq.pop();
        if (vis[u])
            continue;

        vis[u] = 1;
        if (u == t)
            break; // goal reached

        for (auto [v, wt] : G[u])
        {
            if (!vis[v] && g[u] + wt < g[v])
            {
                g[v] = g[u] + wt;

                timer->pause();
                prev[v] = u;
                timer->start();

                pq.emplace(g[v] + w * h[v], v);
            }

            // Logging visited edges
            timer->pause();
            explored_edges.push_back({u, v});
            timer->start();
        }
    }

    timer->pause();
    return g; // g[t] is the path cost
}

int main()
{
    string input = "../input_edges/graph_large_edges.txt";
    string explored_output = "../map_data/graph_large_visited_edges_astar_weighted.txt";
    string path_output = "../map_data/graph_large_final_nodes_astar_weighted.txt";

    Timer runtime;
    Graph G = read_graph(input);
    int s = 0;
    int t = static_cast<int>(G.size()) - 1;
    vector<int> prev;
    std::vector<std::pair<int, int>> explored;

    /* Heuristic: all zeros (admissible but uninformed).
       Replace with domain-specific values for better speed. */
    vector<double> h(G.size(), 0.0);

    auto dist = astar_weighted(G, s, t, h, 1.5, explored, prev, &runtime); // w = 1.5
    write_edges(explored_output, explored);
    write_path(path_output, reconstruct_path(prev, t));

    cout << "Shortest distance: " << dist[t] << "\n";
    cout << "Time: " << runtime.elapsed() << " seconds\n";
}
