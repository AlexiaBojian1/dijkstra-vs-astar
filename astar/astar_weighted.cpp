// Weighted A* (ε-optimal): uses f = g + w·h with w > 1 (e.g., 1.5) to bias the search toward the goal, finding a path no worse than w times optimal much faster.

#include <bits/stdc++.h>
using namespace std;

struct Edge
{
    int to;
    double w;
};
using Graph = vector<vector<Edge>>;
const double INF = numeric_limits<double>::infinity();

Graph read_graph(const string &fname)
{
    ifstream in(fname);
    if (!in)
        throw runtime_error("cannot open " + fname);

    int n, m;
    in >> n >> m;
    Graph G(n);
    for (int i = 0, u, v; i < m; ++i)
    {
        double w;
        in >> u >> v >> w;
        G[u].push_back({v, w});
        G[v].push_back({u, w}); // delete for directed graphs
    }
    return G;
}

/* ---------- Weighted A* ---------- */
vector<double> astar_weighted(const Graph &G,
                              int s, int t,
                              const vector<double> &h,
                              double w = 1.5)
{
    int n = G.size();
    vector<double> g(n, INF);
    vector<char> vis(n, 0);

    using Node = pair<double, int>; // (f,vertex)
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    g[s] = 0;
    pq.emplace(w * h[s], s); // f = g + w·h

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
                pq.emplace(g[v] + w * h[v], v);
            }
        }
    }
    return g; // g[t] is the path cost
}

int main()
{
    const string path = "/home/alexia/algo/dijkstra-vs-astar/data/graph_large.txt";
    Graph G = read_graph(path);

    int s = 0;
    int t = static_cast<int>(G.size()) - 1;

    /* Heuristic: all zeros (admissible but uninformed).
       Replace with domain-specific values for better speed. */
    vector<double> h(G.size(), 0.0);

    auto dist = astar_weighted(G, s, t, h, 1.5); // w = 1.5
    cout << dist[t] << '\n';
}
