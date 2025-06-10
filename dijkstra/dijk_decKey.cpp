//Decrease-key heap Dijkstra: uses a binary (or pairing) heap’s decrease-key operation to update a 
//vertex’s distance in place, giving optimal O((V + E) log V) without wasted inserts.

#include <bits/stdc++.h>
#include <sys/resource.h>          // getrusage
using namespace std;
using Clock = std::chrono::high_resolution_clock;
constexpr char ALG_NAME[] = "dijk_decKey";

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

class MinBinaryHeap
{
    vector<pair<double, int>> h; // (key,vertex)
    vector<int> pos;             // vertex -> index, -1 if not present
    void sift_up(int i)
    {
        while (i && h[i].first < h[(i - 1) / 2].first)
        {
            swap(h[i], h[(i - 1) / 2]);
            pos[h[i].second] = i;
            pos[h[(i - 1) / 2].second] = (i - 1) / 2;
            i = (i - 1) / 2;
        }
    }
    void sift_down(int i)
    {
        int n = h.size();
        while (true)
        {
            int l = 2 * i + 1, r = 2 * i + 2, m = i;
            if (l < n && h[l].first < h[m].first)
                m = l;
            if (r < n && h[r].first < h[m].first)
                m = r;
            if (m == i)
                break;
            swap(h[i], h[m]);
            pos[h[i].second] = i;
            pos[h[m].second] = m;
            i = m;
        }
    }

public:
    explicit MinBinaryHeap(int N) : pos(N, -1) {}
    bool empty() const { return h.empty(); }
    void push(int v, double k)
    {
        h.emplace_back(k, v);
        int i = h.size() - 1;
        pos[v] = i;
        sift_up(i);
    } // insert new vertex
    auto pop()
    {
        auto top = h.front();
        auto last = h.back();
        h[0] = last;
        pos[last.second] = 0;
        h.pop_back();
        if (!h.empty())
            sift_down(0);
        pos[top.second] = -1;
        return top;
    }
    void decrease(int v, double k)
    {
        int i = pos[v];
        if (i == -1 || k >= h[i].first)
            return;
        h[i].first = k;
        sift_up(i);
    } // decrease‑key
    int index(int v) const { return pos[v]; }
};

vector<double> dijkstra_dec_key(const Graph &G, int s, int t = -1)
{
    long nodes = 0;
    auto t0 = Clock::now();

    int n = G.size();
    vector<double> dist(n, INF);
    vector<char> vis(n, 0);
    MinBinaryHeap bh(n);

    dist[s] = 0;
    bh.push(s,0);

    while (!bh.empty())
    {
        auto [d,u] = bh.pop();
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
        for (auto [v,w] : G[u])
            if (!vis[v] && d+w < dist[v]) {
                dist[v] = d+w;
                if (bh.index(v)==-1) bh.push(v, dist[v]);
                else                 bh.decrease(v, dist[v]);
            }
    }

    auto t1 = Clock::now();
    long ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
    cout << "Time(ms): " << ms
         << ", Nodes: "  << nodes
         << ", MaxRSS(KB): " << get_peak_rss_kb()
         << '\n';
    return dist;
}

/* ---------- demo ---------- */
int main()
{
    Graph G = read_graph("/home/alexia/algo/dijkstra-vs-astar/data/graph_huge.txt");
    auto dist =  dijkstra_dec_key(G, 0, G.size()-1);
    cout<<dist[ G.size()-1]<<"\n";
}
