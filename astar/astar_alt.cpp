
#include <bits/stdc++.h>
#include <sys/resource.h>          // getrusage
using namespace std;
using Clock = std::chrono::high_resolution_clock;
constexpr char ALG_NAME[] = "astar_alt";

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
        G[v].push_back({u, w}); // remove if the graph is directed
    }
    return G;
}

//plain Dijkstra (lazy heap) – used for preprocessing
vector<double> dijkstra(const Graph &G, int s)
{
    int n = G.size();
    vector<double> d(n, INF);
    vector<char> vis(n, 0);
    using P = pair<double, int>;
    priority_queue<P, vector<P>, greater<P>> pq;

    d[s] = 0;
    pq.emplace(0, s);

    while (!pq.empty())
    {
        auto [dist, u] = pq.top();
        pq.pop();
        if (vis[u])
            continue;
        vis[u] = 1;

        for (auto [v, w] : G[u])
            if (!vis[v] && dist + w < d[v])
            {
                d[v] = dist + w;
                pq.emplace(d[v], v);
            }
    }
    return d;
}

// farthest-point landmark selection 
vector<int> pick_landmarks(const Graph &G, int k)
{
    int n = G.size();
    vector<int> L;
    L.reserve(k);

    /* first landmark: farthest from vertex 0 */
    vector<double> d0 = dijkstra(G, 0);
    int first = max_element(d0.begin(), d0.end()) - d0.begin();
    L.push_back(first);

    /* repeatedly pick vertex farthest from current landmark set */
    while ((int)L.size() < k)
    {
        vector<double> d_min(n, INF);
        for (int Lidx : L)
        {
            auto dL = dijkstra(G, Lidx);
            for (int v = 0; v < n; ++v)
                d_min[v] = min(d_min[v], dL[v]);
        }
        int nxt = max_element(d_min.begin(), d_min.end()) - d_min.begin();
        L.push_back(nxt);
    }
    return L;
}

// pre-compute single-source distances from each landmark
vector<vector<float>> preprocess_landmarks(const Graph &G,
                                           const vector<int> &L)
{
    int n = G.size(), k = L.size();
    vector<vector<float>> dist(k, vector<float>(n));

    for (int i = 0; i < k; ++i)
    {
        auto d = dijkstra(G, L[i]);
        for (int v = 0; v < n; ++v)
            dist[i][v] = (float)d[v]; // store as 32-bit
    }
    return dist;
}

// ALT heuristic object 
struct MultiALT
{
    int k;
    const vector<vector<float>> dist_from_L; // k × n
    vector<float> distLt;                    // d(L_i, t) for goal t

    MultiALT(vector<vector<float>> &&dist_from_L_, int t)
        : k(dist_from_L_.size()),
          dist_from_L(std::move(dist_from_L_)),
          distLt(k)
    {
        for (int i = 0; i < k; ++i)
            distLt[i] = dist_from_L[i][t];
    }
    inline double operator()(int v) const
    {
        float best = 0;
        for (int i = 0; i < k; ++i)
        {
            float val = fabs(dist_from_L[i][v] - distLt[i]);
            if (val > best)
                best = val;
        }
        return best;
    }
};

// A* search with ALT heuristic
vector<double> astar_best(const Graph &G, int s, int t,
                          const MultiALT &h)            // h(v) pre-computed
{
    long nodes = 0;
    auto t0 = Clock::now();

    int n = G.size();
    vector<double> g(n, INF);
    vector<char> vis(n, 0);
    using Node = pair<double,int>;
    priority_queue<Node,vector<Node>,greater<Node>> pq;

    g[s] = 0;
    pq.emplace(h(s), s);      // f(s)=h(s)

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
                pq.emplace(g[v] + h(v), v);
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


    auto start = std::chrono::high_resolution_clock::now();
    int s = 0;
    int t = static_cast<int>(G.size()) - 1;

    int k = 8; // number of landmarks
    auto L = pick_landmarks(G, k);
    auto distL = preprocess_landmarks(G, L);
    MultiALT h(std::move(distL), t);

    auto d = astar_best(G, s, t, h);
    cout << d[t] << '\n'; // print distance
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Time(ms): " << ms << std::endl;
 }
