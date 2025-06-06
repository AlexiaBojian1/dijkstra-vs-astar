//Lazy binary-heap Dijkstra: pushes every tentative edge cost into a binary heap and simply ignores outdated entries when popped, 
//trading extra inserts for simpler code.
#include <bits/stdc++.h>
#include <fstream>

class Timer {
public:
	Timer() {
		start_time = std::chrono::system_clock::now();
	}
	std::chrono::duration<double> elapsed() const {
		return (std::chrono::system_clock::now() - start_time);
	}
private:
	std::chrono::time_point<std::chrono::system_clock> start_time;
};

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

vector<double> dijkstra_lazy(const Graph &G, int s, int t = -1)
{
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
    Graph G = read_graph("../data/graph_large.txt");
    auto dist = dijkstra_lazy(G, 0, G.size()-1);
    cout<<dist[ G.size()-1]<<"\n";
	cout << "time = " << runtime.elapsed().count() << '\n';

}