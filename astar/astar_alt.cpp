//ALT / Landmark A*: precomputes distances to a landmark L and sets h(s)=|d(L,s)−d(L,goal)|,
// a triangle-inequality heuristic that tightens estimates without losing admissibility.

#include "../helpers/timer.h"
#include "../helpers/graph_io.h"
#include <iostream>

#include <bits/stdc++.h>

using namespace std;
const double INF = numeric_limits<double>::infinity();

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
vector<double> astar_best(const Graph &G,
                          int s, int t,
                          const MultiALT &h,
                          std::vector<std::pair<int, int>>& explored_edges, 
                          vector<int>& prev, 
                          Timer* timer)
{
    timer->start();
    int n = G.size();
    vector<double> g(n, INF);
    vector<char> vis(n, 0);
    using P = pair<double, int>;
    priority_queue<P, vector<P>, greater<P>> pq;

    g[s] = 0;
    pq.emplace(h(s), s); // f(s)=h(s)

    // For path reconstruction
    timer->pause();
    prev.assign(n, -1);
    timer->start();


    while (!pq.empty())
    {
        auto [f, u] = pq.top();
        pq.pop();
        if (vis[u])
            continue;

        vis[u] = 1;
        if (u == t)
            break; // goal reached

        for (auto [v, w] : G[u]) {
            if (!vis[v] && g[u] + w < g[v])
            {
                g[v] = g[u] + w;

                timer->pause();
                prev[v] = u;
                timer->start();

                pq.emplace(g[v] + h(v), v);
            }
            // Logging visited edges
            timer->pause();
            explored_edges.push_back({u, v});
            timer->start();
        } 
    }

    timer->pause();
    return g; // g[t] holds distance
}

int main()
{
    vector<pair<string, int>> datasets = {
        {"large", 0},
        {"Netherlands", 60000}
    };

    for (const auto& [name, source] : datasets) {

        string input = "../input_edges/graph_" + name + "_edges.txt";
        string explored_output = "../map_data/graph_" + name + "_visited_edges_astar_alt.txt";
        string path_output = "../map_data/graph_" + name + "_final_nodes_astar_alt.txt";

        Timer runtime;
        Graph G = read_graph(input);
        int s = source;
        int t = static_cast<int>(G.size()) - 1;
        vector<int> prev;
        std::vector<std::pair<int, int>> explored;

        int k = 8; // number of landmarks
        auto L = pick_landmarks(G, k);
        auto distL = preprocess_landmarks(G, L);
        MultiALT h(std::move(distL), t);

        auto dist = astar_best(G, s, t, h, explored, prev, &runtime);
        write_edges(explored_output, explored);
        write_path(path_output, reconstruct_path(prev, t));

        cout << "Shortest distance (" << name << "): " << dist[t] << "\n";
        cout << "Time (" << name << "): " << runtime.elapsed() << " seconds\n";
    } 
}
