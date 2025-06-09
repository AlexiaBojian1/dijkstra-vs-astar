//Decrease-key heap Dijkstra: uses a binary (or pairing) heap’s decrease-key operation to update a 
//vertex’s distance in place, giving optimal O((V + E) log V) without wasted inserts.

#include "../helpers/timer.h"
#include "../helpers/graph_io.h"
#include <iostream>
#include <bits/stdc++.h>

using namespace std;
const double INF = numeric_limits<double>::infinity();

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

vector<double> dijkstra_dec_key(const Graph &G, int s, int t, std::vector<std::pair<int, int>>& explored_edges, vector<int>& prev, Timer* timer) 
{
    timer->start();
    int n = G.size();
    vector<double> dist(n, INF);
    vector<char> vis(n, 0);
    MinBinaryHeap bh(n);
    dist[s] = 0;
    bh.push(s, 0);

    // For path reconstruction
    timer->pause();
    prev.assign(n, -1);
    timer->start();

    while (!bh.empty())
    {
        auto [d, u] = bh.pop();
        if (vis[u])
            continue;
        vis[u] = 1;
        if (u == t)
            break;
        for (auto [v, w] : G[u]) {
            if (!vis[v] && d + w < dist[v])
            {
                dist[v] = d + w;

                timer->pause();
                prev[v] = u;
                timer->start();

                if (bh.index(v) == -1)
                    bh.push(v, dist[v]);
                else
                    bh.decrease(v, dist[v]);
            }
            
            // Logging visited edges
            timer->pause();
            explored_edges.push_back({u, v});
            timer->start();
        }
    }
    timer->pause();
    return dist;
}

/* ---------- demo ---------- */
int main()
{
    string input = "../input_edges/graph_large_edges.txt";
    string explored_output = "../map_data/graph_large_visited_edges_dijk_generated.txt";
    string path_output = "../map_data/graph_large_final_nodes_dijk_generated.txt";

    Timer runtime;
    Graph G = read_graph(input);
    int s = 0, t = G.size() - 1;
    vector<int> prev;
    std::vector<std::pair<int, int>> explored;

    auto dist = dijkstra_dec_key(G, s, t, explored, prev, &runtime);
    write_edges(explored_output, explored);
    write_path(path_output, reconstruct_path(prev, t));

    cout << "Shortest distance: " << dist[t] << "\n";
    cout << "Time: " << runtime.elapsed() << " seconds\n";
}
