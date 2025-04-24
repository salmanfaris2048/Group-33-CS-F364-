#include <bits/stdc++.h>

using namespace std;

struct Graph {
    int n;
    vector<vector<int>> adj;

    Graph(int size) : n(size), adj(size) {}

    void addEdge(int u, int v) {
        if (u >= n || v >= n || u < 0 || v < 0) {
            cerr << "Invalid edge: " << u + 1 << " - " << v + 1 << endl;
            exit(1);
        }
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    static Graph fromFile(const string& filename) {
        ifstream in(filename);
        if (!in) {
            cerr << "Cannot open file: " << filename << endl;
            exit(1);
        }

        int n, m;
        in >> n >> m;
        Graph G(n);
        for (int i = 0; i < m; ++i) {
            int u, v;
            in >> u >> v;
            --u; --v; // Adjust for 1-based indexing in the input
            if (u < 0 || v < 0 || u >= n || v >= n) {
                cerr << "Invalid edge in input file (1-based): " << (u + 1) << " - " << (v + 1) << endl;
                exit(1);
            }
            G.addEdge(u, v);
        }

        for (auto& neighbors : G.adj)
            sort(neighbors.begin(), neighbors.end());

        return G;
    }

    void findCliques(int h, vector<vector<int>>& cliques, const set<int>& nodes) {
        vector<int> nodeList(nodes.begin(), nodes.end());
        int local_n = nodeList.size();
        vector<int> current;
        function<void(int, int)> extendClique = [&](int idx, int depth) {
            if (depth == h) {
                cliques.push_back(current);
                return;
            }
            for (int i = idx + 1; i < local_n; ++i) {
                int u = nodeList[i];
                bool valid = true;
                for (int w : current)
                    if (!binary_search(adj[u].begin(), adj[u].end(), w)) {
                        valid = false;
                        break;
                    }
                if (valid) {
                    current.push_back(u);
                    extendClique(i, depth + 1);
                    current.pop_back();
                }
            }
        };

        for (int i = 0; i < local_n; ++i) {
            current = {nodeList[i]};
            extendClique(i, 1);
        }
    }

    double computeHCliqueDensity(const set<int>& nodes, int h) {
        vector<vector<int>> cliques;
        findCliques(h, cliques, nodes);
        int numCliques = cliques.size();
        int size = nodes.size();
        if (size < h) return 0.0;
        double denom = 1.0;
        for (int i = 0; i < h; ++i)
            denom *= (size - i);
        denom /= tgamma(h + 1); // binomial coefficient denominator: h!

        double rho = numCliques / denom;
        cout << "[DEBUG] h: " << h << ", #nodes: " << size << ", #cliques: " << numCliques << ", density (rho): " << rho << "\n";

        return rho;
    }

    vector<int> hCliqueCoreDecomposition(int h) {
        vector<vector<int>> cliques;
        set<int> allNodes;
        for (int i = 0; i < n; ++i) allNodes.insert(i);
        findCliques(h, cliques, allNodes);

        vector<int> core(n, 0);
        vector<int> cliqueCount(n, 0);
        for (auto& clique : cliques)
            for (int v : clique)
                cliqueCount[v]++;

        set<pair<int, int>> pq;
        for (int i = 0; i < n; ++i)
            pq.insert({cliqueCount[i], i});

        vector<bool> removed(n, false);

        while (!pq.empty()) {
            auto [deg, u] = *pq.begin();
            pq.erase(pq.begin());
            core[u] = deg;
            removed[u] = true;

            for (auto& clique : cliques) {
                if (find(clique.begin(), clique.end(), u) == clique.end()) continue;
                for (int v : clique) {
                    if (v != u && !removed[v] && pq.count({cliqueCount[v], v})) {
                        pq.erase({cliqueCount[v], v});
                        cliqueCount[v]--;
                        pq.insert({cliqueCount[v], v});
                    }
                }
            }
        }

        return core;
    }

    set<set<int>> getConnectedComponents(const vector<int>& core, int k) {
        vector<bool> visited(n, false);
        set<set<int>> components;

        function<void(int, set<int>&)> dfs = [&](int u, set<int>& comp) {
            visited[u] = true;
            comp.insert(u);
            for (int v : adj[u]) {
                if (!visited[v] && core[v] >= k)
                    dfs(v, comp);
            }
        };

        for (int i = 0; i < n; ++i) {
            if (!visited[i] && core[i] >= k) {
                set<int> comp;
                dfs(i, comp);
                components.insert(comp);
            }
        }
        return components;
    }

    int computeInitialK0(int h) {
        set<int> allNodes;
        for (int i = 0; i < n; ++i) allNodes.insert(i);
        double rho0 = computeHCliqueDensity(allNodes, h);
        return ceil(rho0);
    }

    int refineK0WithPruning2(const vector<int>& core, int k0, int h) {
        set<set<int>> components = getConnectedComponents(core, k0);
        double rho_max = 0.0;
        for (const auto& comp : components) {
            double rho = computeHCliqueDensity(comp, h);
            rho_max = max(rho_max, rho);
        }
        int k00 = ceil(rho_max);
        if (k00 > k0) return k00;
        return k0;
    }
};


struct FlowEdge {
    int v, u;
    long long cap, flow = 0;
    FlowEdge(int v, int u, long long cap) : v(v), u(u), cap(cap) {}
};

struct FlowNetwork {
    int n;
    vector<FlowEdge> edges;
    vector<vector<int>> adj;
    vector<int> level, ptr;
    queue<int> q;

    FlowNetwork(int n) : n(n), adj(n), level(n), ptr(n) {}

    void addEdge(int v, int u, long long cap) {
        edges.emplace_back(v, u, cap);
        edges.emplace_back(u, v, 0); // reverse edge
        adj[v].push_back(edges.size() - 2);
        adj[u].push_back(edges.size() - 1);
    }

    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);
        level[s] = 0;
        q.push(s);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (int id : adj[v]) {
                if (edges[id].cap - edges[id].flow < 1) continue;
                if (level[edges[id].u] == -1) {
                    level[edges[id].u] = level[v] + 1;
                    q.push(edges[id].u);
                }
            }
        }
        return level[t] != -1;
    }

    long long dfs(int v, int t, long long pushed) {
        if (pushed == 0) return 0;
        if (v == t) return pushed;
        for (int& cid = ptr[v]; cid < (int)adj[v].size(); ++cid) {
            int id = adj[v][cid];
            int u = edges[id].u;
            if (level[v] + 1 != level[u] || edges[id].cap - edges[id].flow < 1) continue;
            long long tr = dfs(u, t, min(pushed, edges[id].cap - edges[id].flow));
            if (tr == 0) continue;
            edges[id].flow += tr;
            edges[id ^ 1].flow -= tr;
            return tr;
        }
        return 0;
    }

    long long maxFlow(int s, int t) {
        long long flow = 0;
        while (true) {
            if (!bfs(s, t)) break;
            fill(ptr.begin(), ptr.end(), 0);
            while (long long pushed = dfs(s, t, LLONG_MAX)) {
                flow += pushed;
            }
        }
        return flow;
    }

    vector<bool> minCut(int s) {
        vector<bool> visited(n, false);
        queue<int> q;
        q.push(s);
        visited[s] = true;
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (int id : adj[v]) {
                int u = edges[id].u;
                if (edges[id].cap - edges[id].flow > 0 && !visited[u]) {
                    visited[u] = true;
                    q.push(u);
                }
            }
        }
        return visited;
    }
};

// Stage 3: CDS via Binary Search on alpha

set<int> extractCDS(set<int>& component, const vector<vector<int>>& cliques, int h, double eps = 1e-6) {
    int N = component.size();
    map<int, int> id; vector<int> rev(N);
    int idx = 0;
    for (int v : component) {
        id[v] = idx;
        rev[idx] = v;
        ++idx;
    }

    double l = 0, u = component.size();
    set<int> bestCDS = component;
    while (u - l > eps) {
        double alpha = (l + u) / 2.0;
        FlowNetwork net(N + 2);
        int S = N, T = N + 1;

        vector<int> count(N, 0);
        for (const auto& clique : cliques) {
            bool inside = true;
            for (int v : clique)
                if (!component.count(v)) { inside = false; break; }
            if (!inside) continue;
            for (int v : clique) count[id[v]]++;
        }

        for (int i = 0; i < N; ++i) {
            net.addEdge(S, i, count[i]);
            net.addEdge(i, T, alpha);
        }

        net.maxFlow(S, T);
        vector<bool> cut = net.minCut(S);
        set<int> currentCDS;
        for (int i = 0; i < N; ++i)
            if (cut[i]) currentCDS.insert(rev[i]);

        if (!currentCDS.empty()) {
            bestCDS = currentCDS;
            l = alpha;
        } else {
            u = alpha;
        }
    }
    return bestCDS;
}

#include <fstream>

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <graph_file.txt> <h>" << endl;
        return 1;
    }

    string filename = argv[1];
    int h = stoi(argv[2]);

    Graph G = Graph::fromFile(filename);
    int k0 = G.computeInitialK0(h);
    vector<int> core = G.hCliqueCoreDecomposition(h);
    int k_final = G.refineK0WithPruning2(core, k0, h);

    cout << "Initial k0: " << k0 << endl;
    cout << "Refined k0 (after Pruning2): " << k_final << endl;

    auto components = G.getConnectedComponents(core, k_final);
    set<int> bestCDS;
    double bestDensity = -1;

    for (const auto& comp : components) {
        double density = G.computeHCliqueDensity(comp, h);
        cout << "Component size: " << comp.size() << ", h-clique-density: " << density << endl;
        vector<vector<int>> cliques;
        G.findCliques(h, cliques, comp);
        set<int> cds = extractCDS((set<int>&)comp, cliques, h);
        double cdsDensity = G.computeHCliqueDensity(cds, h);
        if (cdsDensity > bestDensity) {
            bestDensity = cdsDensity;
            bestCDS = cds;
        }
    }

    cout << endl << "Densest Subgraph (CDS) with h = " << h << " has density: " << bestDensity << endl;
    cout << "CDS vertices (1-based):" << endl;
    for (int v : bestCDS) cout << (v + 1) << " ";
    cout << endl;

    return 0;
}
