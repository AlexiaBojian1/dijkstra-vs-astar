#include <bits/stdc++.h>
#include <sys/resource.h>
using namespace std;
using Clock = chrono::high_resolution_clock;

constexpr char ALG_NAME[] = "astar_weighted";

inline long peak_rss_kb() {
    rusage u{}; getrusage(RUSAGE_SELF, &u);
    return u.ru_maxrss;            // kB
}

struct Edge { int to; double w; };
using  Graph = vector<vector<Edge>>;
struct Pt { double x, y; };

Graph read_graph(const string& path, vector<Pt>& coord)
{
    ifstream in(path);
    if(!in) throw runtime_error("cannot open " + path);

    int n, m;              
    in >> n >> m;
    coord.resize(n);
    for(int i = 0; i < n; ++i) in >> coord[i].x >> coord[i].y;

    Graph G(n);
    for(int i = 0, u, v; i < m; ++i){
        double w; in >> u >> v >> w;
        G[u].push_back({v,w});
        G[v].push_back({u,w});
    }
    return G;
}

double euclid(int v, int t, const vector<Pt>& p){
    double dx = p[v].x - p[t].x, dy = p[v].y - p[t].y;
    return sqrt(dx*dx + dy*dy);
}


vector<double> astar_weighted(const Graph& G, int s, int t,
                              const vector<double>& h, double w = 1.5)
{
    const double INF = numeric_limits<double>::infinity();
    int n = G.size();
    long nodes = 0;

    vector<double> g(n, INF);
    vector<char>   closed(n, 0);

    using Node = pair<double,int>; 
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    g[s] = 0;
    pq.emplace(g[s] + w * h[s], s);

    long nodes_expanded = 0;
    auto t0 = Clock::now();

    while(!pq.empty()){
        auto [f,u] = pq.top(); pq.pop();
        if(closed[u]) continue;
        closed[u] = 1;
        ++nodes_expanded;
        nodes++;
        static const long SAMPLE = 1000;     
        static std::ofstream trace(std::string("trace_") + ALG_NAME + ".csv");
        if (nodes % SAMPLE == 0) {
            auto now  = Clock::now();
            long ms   = std::chrono::duration_cast<std::chrono::milliseconds>(now - t0).count();
            trace << ms << ',' << nodes << '\n';
        }

        if(u == t) break;

        for(auto [v,c] : G[u])
            if(!closed[v] && g[u] + c < g[v]) {
                g[v] = g[u] + c;
                pq.emplace(g[v] + w * h[v], v);
            }
    }
    auto ms = chrono::duration_cast<chrono::milliseconds>(
                  Clock::now() - t0).count();
    cout << "Time(ms): " << ms
         << ", Nodes: " << nodes_expanded
         << ", MaxRSS(KB): " << peak_rss_kb() << '\n';

    return g;
}


int main()
{
    const string path = "/home/alexia/algo/dijkstra-vs-astar/data/graph_astar_weighted.txt";
    vector<Pt> coord;
    Graph G = read_graph(path, coord);

    int s = 0, t = (int)G.size() - 1;

    //precompute heuristic h(v) = straight-line distance to goal 
    vector<double> h(G.size());
    for(size_t v = 0; v < G.size(); ++v)
        h[v] = euclid((int)v, t, coord);  

    double weight = 1.5;                
    auto dist = astar_weighted(G, s, t, h, weight);

    cout << "dist(s, t) = " << dist[t] << '\n';
}
