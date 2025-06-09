// g++ -std=c++17 -Iinclude dijkstra/dijk_lazy.cpp helpers/timer.cpp -o build/dijkstra_lazy_large
// ./build/dijkstra_lazy_large.exe
//Lazy binary-heap Dijkstra: pushes every tentative edge cost into a binary heap and simply ignores outdated entries when popped, 
//trading extra inserts for simpler code.
#include "../helpers/timer.h"
#include <iostream>
#include <bits/stdc++.h>
#include <fstream>

using namespace std;
struct Edge
{
    int to;
    double w;
};
using Graph = vector<vector<Edge>>;
const double INF = numeric_limits<double>::infinity();

Graph read_graph(const string& fname) {
    ifstream in(fname);
    int n, m;
    in>> n >> m;
    Graph G(n);
    for(int i = 0, u, v; i < m; i++) {
        double w;
        in >> u >> v >> w;
        G[u].push_back({v , w});
        G[v].push_back({u , w});
    }
    return G;
}

vector<double> dijkstra_lazy(const Graph &G, int s, int t = -1, Timer* timer = nullptr)
{
    timer->start();

    int n = G.size();
    vector<double> dist(n, INF);
    vector<char> vis(n, 0);
    using Node = pair<double, int>;
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    dist[s] = 0;
    pq.emplace(0, s);

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
                pq.emplace(dist[v], v);
            }
        }
    }

    timer->pause();
    return dist;
}

int main()
{
    // Graph G(4);
    // G[0] = {{1, 1}, {2, 4}};
    // G[1] = {{2, 2}, {3, 5}};
    // G[2] = {{3, 1}};
    // auto d = dijkstra_lazy(G, 0);
    // cout << "dist(0->3)=" << d[3] << "\n"; // should print 4
    Timer runtime;
    Graph G = read_graph("../map_data/graph_large_edges.txt");
    auto dist = dijkstra_lazy(G, 0, G.size()-1, &runtime);
    cout<<dist[ G.size()-1]<<"\n";
	cout << "time = " << runtime.elapsed() << " seconds\n";
}