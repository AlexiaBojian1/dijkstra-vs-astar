#include <bits/stdc++.h>
#include <boost/heap/fibonacci_heap.hpp>
using namespace std;
struct Edge
{
    int to;
    double w;
};
using Graph = vector<vector<Edge>>;
const double INF = numeric_limits<double>::infinity();

vector<double> dijkstra_fib(const Graph &G, int s, int t = -1)
{
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
            if (!vis[v] && d + w < dist[v])
            {
                dist[v] = d + w;
                if (ref[v] == Handle())
                    ref[v] = pq.push({dist[v], v});
                else
                    pq.update(ref[v], {dist[v], v});
            }
    }
    return dist;
}

/* ---------- demo ---------- */
int main()
{
    Graph G(4);
    G[0] = {{1, 1}, {2, 4}};
    G[1] = {{2, 2}, {3, 5}};
    G[2] = {{3, 1}};
    auto d = dijkstra_fib(G, 0);
    cout << "dist(0->3)=" << d[3] << "\n"; // 4
}