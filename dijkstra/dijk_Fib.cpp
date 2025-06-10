//Fibonacci-heap Dijkstra: replaces the heap with a Fibonacci heap so decrease-key is amortized O(1), 
//dropping the total time to O(E + V log V) at the cost of higher constant factors.

#include <bits/stdc++.h>
#include <boost/heap/fibonacci_heap.hpp>
#include <sys/resource.h>          // getrusage
using namespace std;
using Clock = std::chrono::high_resolution_clock;
constexpr char ALG_NAME[] = "dijk_Fib";

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

vector<double> dijkstra_fib(const Graph &G, int s, int t = -1)
{
    long nodes = 0;
    auto t0 = Clock::now();

    using Node = pair<double,int>;
    using Fib  = boost::heap::fibonacci_heap<Node, boost::heap::compare<greater<Node>>>;
    using Handle = Fib::handle_type;

    int n = G.size();
    vector<double> dist(n, INF);
    vector<char>   vis(n, 0);
    vector<Handle> ref(n);            // track handles
    Fib pq;

    dist[s] = 0;
    ref[s] = pq.push({0,s});

    while (!pq.empty())
    {
        auto [d,u] = pq.top(); pq.pop();
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
                if (ref[v]==Handle()) ref[v] = pq.push({dist[v],v});
                else                  pq.update(ref[v], {dist[v],v});
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
    auto dist =  dijkstra_fib(G, 0, G.size()-1);
    cout<<dist[ G.size()-1]<<"\n";
}