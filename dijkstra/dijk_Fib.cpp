//Fibonacci-heap Dijkstra: replaces the heap with a Fibonacci heap so decrease-key is amortized O(1), 
//dropping the total time to O(E + V log V) at the cost of higher constant factors.

#include "../helpers/timer.h"
#include "../helpers/graph_io.h"
#include <iostream>
#include <bits/stdc++.h>
#include <boost/heap/fibonacci_heap.hpp>

using namespace std;
const double INF = numeric_limits<double>::infinity();

vector<double> dijkstra_fib(const Graph &G, int s, int t, std::vector<std::pair<int, int>>& explored_edges, vector<int>& prev, Timer* timer)
{
    timer->start();
    using Node = pair<double, int>;
    using Fib = boost::heap::fibonacci_heap<Node, boost::heap::compare<greater<Node>>>;
    using Handle = Fib::handle_type;
    int n = G.size();
    vector<double> dist(n, INF);
    vector<char> vis(n, 0);
    vector<Handle> ref(n);
    Fib pq;
    dist[s] = 0;
    ref[s] = pq.push({0, s});

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
        for (auto [v, w] : G[u]) {
            if (!vis[v] && d + w < dist[v])
            {
                dist[v] = d + w;

                timer->pause();
                prev[v] = u;
                timer->start();

                if (ref[v] == Handle())
                    ref[v] = pq.push({dist[v], v});
                else
                    pq.update(ref[v], {dist[v], v});
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

/* ---------- demo ---------- */
int main()
{
    string input = "../input_edges/graph_large_edges.txt";
    string explored_output = "../map_data/graph_large_visited_edges_dijk_Fib.txt";
    string path_output = "../map_data/graph_large_final_nodes_dijk_Fib.txt";

    Timer runtime;
    Graph G = read_graph(input);
    int s = 0, t = G.size() - 1;
    vector<int> prev;
    std::vector<std::pair<int, int>> explored;

    auto dist = dijkstra_fib(G, s, t, explored, prev, &runtime);
    write_edges(explored_output, explored);
    write_path(path_output, reconstruct_path(prev, t));

    cout << "Shortest distance: " << dist[t] << "\n";
    cout << "Time: " << runtime.elapsed() << " seconds\n";
}