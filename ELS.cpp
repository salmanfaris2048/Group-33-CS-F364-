#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <chrono>
#include <omp.h>

using namespace std;

vector<vector<int>> graph;
vector<int> degeneracyOrder;
vector<int> idToOriginal;
unordered_map<int, int> idMap;

int printed_cliques = 0;
const int max_cliques = INT32_MAX;
const int min_clique_size = 3;

ofstream sizefile("clique_sizes.txt");
ofstream outfile("test1.txt");
int max_clique_size = 0;

#pragma omp critical
void output_clique(const vector<int>& R) {
    if ((int)R.size() >= min_clique_size && printed_cliques < max_cliques) {
        printed_cliques++;
        for (size_t i = 0; i < R.size(); ++i) {
            outfile << idToOriginal[R[i]] << (i + 1 == R.size() ? "\n" : " ");
        }
        sizefile << R.size() << endl;

        #pragma omp critical (maxsize)
        if ((int)R.size() > max_clique_size) {
            max_clique_size = R.size();
        }
    }
}

vector<int> intersect_sorted(const vector<int>& a, const vector<int>& b) {
    vector<int> result;
    set_intersection(a.begin(), a.end(), b.begin(), b.end(), back_inserter(result));
    return result;
}

void bronKerboschPivotFast(const vector<vector<int>>& graph, vector<int>& R, const vector<int>& P, const vector<int>& X) {
    if (P.empty() && X.empty()) {
        output_clique(R);
        return;
    }

    int pivot = -1, max_deg = -1;
    unordered_set<int> Pset(P.begin(), P.end());
    unordered_set<int> Xset(X.begin(), X.end());
    unordered_set<int> unionPX = Pset;
    unionPX.insert(Xset.begin(), Xset.end());

    for (int u : unionPX) {
        int deg = 0;
        for (int v : graph[u]) {
            if (Pset.count(v)) deg++;
        }
        if (deg > max_deg) {
            max_deg = deg;
            pivot = u;
        }
    }

    unordered_set<int> neighbors(graph[pivot].begin(), graph[pivot].end());
    vector<int> candidates;
    for (int v : P) {
        if (!neighbors.count(v)) candidates.push_back(v);
    }

    for (int v : candidates) {
        vector<int> Rnew = R;
        Rnew.push_back(v);
        vector<int> Pnew = intersect_sorted(P, graph[v]);
        vector<int> Xnew = intersect_sorted(X, graph[v]);
        bronKerboschPivotFast(graph, Rnew, Pnew, Xnew);
        if (printed_cliques >= max_cliques) return;
    }
}

void runEPSListing(int n) {
    vector<int> position(n);
    for (int i = 0; i < n; ++i) position[degeneracyOrder[i]] = i;

    #pragma omp parallel for schedule(dynamic)
    for (int idx = 0; idx < n; ++idx) {
        int v = degeneracyOrder[idx];
        vector<int> P, X;
        for (int u : graph[v]) {
            if (position[u] > position[v]) P.push_back(u);
            else if (position[u] < position[v]) X.push_back(u);
        }
        sort(P.begin(), P.end());
        sort(X.begin(), X.end());
        vector<int> R = {v};
        bronKerboschPivotFast(graph, R, P, X);
    }
}

int main() {
    string filename = "Wiki-Vote.txt";
    ifstream infile(filename);
    if (!infile) {
        cerr << "Error opening file " << filename << endl;
        return 1;
    }

    unordered_set<int> vertexSet;
    vector<pair<int, int>> edges;
    string line;
    while (getline(infile, line)) {
        if (line.empty() || line[0] == '#') continue;
        istringstream iss(line);
        int u, v;
        if (!(iss >> u >> v)) continue;
        if (u == v) continue;
        edges.emplace_back(u, v);
        edges.emplace_back(v, u);
        vertexSet.insert(u);
        vertexSet.insert(v);
    }
    infile.close();

    vector<int> vertices(vertexSet.begin(), vertexSet.end());
    sort(vertices.begin(), vertices.end());
    int n = vertices.size();

    graph.resize(n);
    idToOriginal.resize(n);
    for (int i = 0; i < n; ++i) {
        idMap[vertices[i]] = i;
        idToOriginal[i] = vertices[i];
    }

    for (auto& [u, v] : edges) {
        if (idMap.count(u) && idMap.count(v)) {
            int a = idMap[u], b = idMap[v];
            graph[a].push_back(b);
        }
    }
    for (int i = 0; i < n; ++i) sort(graph[i].begin(), graph[i].end());

    vector<int> degrees(n);
    vector<bool> removed(n, false);
    degeneracyOrder.reserve(n);
    for (int i = 0; i < n; ++i) degrees[i] = graph[i].size();
    for (int i = 0; i < n; ++i) {
        int minDeg = n + 1, u = -1;
        for (int j = 0; j < n; ++j) {
            if (!removed[j] && degrees[j] < minDeg) {
                minDeg = degrees[j];
                u = j;
            }
        }
        if (u == -1) break;
        removed[u] = true;
        degeneracyOrder.push_back(u);
        for (int v : graph[u]) if (!removed[v]) degrees[v]--;
    }

    auto start = chrono::high_resolution_clock::now();
    runEPSListing(n);
    auto end = chrono::high_resolution_clock::now();

    outfile.close();
    sizefile.close();

    cout << "Finished in "
         << chrono::duration_cast<chrono::seconds>(end - start).count()
         << " seconds. Total cliques printed: " << printed_cliques << endl;
    cout << "Max clique size: " << max_clique_size << endl;

    return 0;
}
