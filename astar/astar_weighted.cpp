// Weighted A* (ε-optimal): uses f = g + w·h with w > 1 (e.g., 1.5) to bias the search toward the goal, finding a path no worse than w times optimal much faster.

#include <bits/stdc++.h>
#include <sys/resource.h>          // getrusage
using namespace std;
using Clock = std::chrono::high_resolution_clock;
constexpr char ALG_NAME[] = "astar_weighted";


inline long get_peak_rss_kb() {
    struct rusage usage{};
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;        // kilobytes
}

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
vector<double> astar_weighted(const Graph &G, int s, int t,
                              const vector<double> &h, double w = 1.5)
{
    long nodes = 0;
    auto t0 = Clock::now();

    int n = G.size();
    vector<double> g(n, INF);
    vector<char> vis(n, 0);
    using Node = pair<double,int>;
    priority_queue<Node,vector<Node>,greater<Node>> pq;

    g[s] = 0;
    pq.emplace(w*h[s], s);    // f(s)=g+h

    while (!pq.empty())
    {
        auto [f,u] = pq.top(); pq.pop();
        if (vis[u]) continue;
        vis[u] = 1;

        nodes++;
        static const long SAMPLE = 1000;          // tweak if graph is tiny/huge
        static std::ofstream trace(std::string("trace_") + ALG_NAME + ".csv");
        if (nodes % SAMPLE == 0) {
            auto now  = Clock::now();
            long ms   = std::chrono::duration_cast<std::chrono::milliseconds>(now - t0).count();
            trace << ms << ',' << nodes << '\n';
        }

        if (u==t) break;
        for (auto [v,wuv] : G[u])
            if (!vis[v] && g[u]+wuv < g[v]) {
                g[v] = g[u]+wuv;
                pq.emplace(g[v] + w*h[v], v);
            }
    }

    auto t1 = Clock::now();
    long ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
    cout << "Time(ms): " << ms
         << ", Nodes: "  << nodes
         << ", MaxRSS(KB): " << get_peak_rss_kb()
         << '\n';
    return g;
}


int main()
{
    const string path = "/home/alexia/algo/dijkstra-vs-astar/data/graph_huge.txt";
    Graph G = read_graph(path);

    int s = 0;
    int t = static_cast<int>(G.size()) - 1;

    /* Heuristic: all zeros (admissible but uninformed).
       Replace with domain-specific values for better speed. */
    vector<double> h(G.size(), 0.0);

    auto dist = astar_weighted(G, s, t, h, 1.5); // w = 1.5
    cout << dist[t] << '\n';
}
