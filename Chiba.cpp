#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <set>
#include <chrono>
#include <bitset>
#include <omp.h>

using namespace std;

int max_cliques = INT32_MAX;
int min_clique_size = 3;
int printed_cliques = 0;

ofstream sizefile("clique_sizesE.txt"); // File to store clique sizes

int max_clique_size = 0;
ofstream outfile("test_chibaE.txt");
ofstream distfile("Clique_dist.txt");
unordered_map<int, int> cliqueSizeFreq;

#pragma omp critical
void output_clique(const vector<int>& R, const vector<int>& idToOriginal) {
    if ((int)R.size() >= min_clique_size && printed_cliques < max_cliques) {
        printed_cliques++;

        // Write the clique to the main output file
        for (size_t i = 0; i < R.size(); ++i) {
            outfile << idToOriginal[R[i]] << (i + 1 == R.size() ? "\n" : " ");
        }


        // Write the clique size to the size file
        sizefile << R.size() << endl;
        #pragma omp critical
        {
        if ((int)R.size() > max_clique_size) {
            max_clique_size = R.size();
        }
        cliqueSizeFreq[R.size()]++;
        }
    }
}

void extend_clique(const vector<vector<bool>>& graph, vector<int>& R, const vector<int>& cand, const vector<int>& id_to_original) {
    output_clique(R, id_to_original);
    for (size_t i = 0; i < cand.size(); ++i) {
        int v = cand[i];
        vector<int> R_new = R;
        R_new.push_back(v);

        vector<int> new_cand;
        for (size_t j = i + 1; j < cand.size(); ++j) {
            if (graph[v][cand[j]]) new_cand.push_back(cand[j]);
        }
        extend_clique(graph, R_new, new_cand, id_to_original);
        if (printed_cliques >= max_cliques) return;
    }
}

void chiba_clique_listing(const vector<vector<bool>>& graph, const vector<int>& degeneracy_order, const vector<int>& id_to_original) {
    int n = graph.size();
    vector<int> position(n);
    for (int i = 0; i < n; ++i) position[degeneracy_order[i]] = i;

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; ++i) {
        int v = degeneracy_order[i];
        vector<int> cand;
        for (int u = 0; u < n; ++u) {
            if (graph[v][u] && position[u] > position[v]) cand.push_back(u);
        }
        vector<int> R = {v};
        extend_clique(graph, R, cand, id_to_original);
    }
}

int main() {
    string filename = "Email-Enron.txt";
    ifstream infile(filename);
    if (!infile) {
        cerr << "Error opening file " << filename << endl;
        return 1;
    }

    unordered_set<int> vertex_set;
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
        vertex_set.insert(u);
        vertex_set.insert(v);
    }
    infile.close();

    vector<int> vertices(vertex_set.begin(), vertex_set.end());
    sort(vertices.begin(), vertices.end());
    unordered_map<int, int> id_map;
    vector<int> id_to_original(vertices.size());
    for (int i = 0; i < vertices.size(); ++i) {
        id_map[vertices[i]] = i;
        id_to_original[i] = vertices[i];
    }

    int n = vertices.size();
    vector<vector<bool>> graph(n, vector<bool>(n, false));
    for (auto& [u, v] : edges) {
        if (id_map.count(u) && id_map.count(v)) {
            int a = id_map[u], b = id_map[v];
            graph[a][b] = true;
        }
    }

    // Degeneracy ordering
    vector<int> degrees(n);
    vector<bool> removed(n, false);
    vector<int> degeneracy_order;
    for (int i = 0; i < n; ++i) degrees[i] = count(graph[i].begin(), graph[i].end(), true);
    for (int i = 0; i < n; ++i) {
        int min_deg = n + 1, u = -1;
        for (int j = 0; j < n; ++j) {
            if (!removed[j] && degrees[j] < min_deg) {
                min_deg = degrees[j];
                u = j;
            }
        }
        if (u == -1) break;
        removed[u] = true;
        degeneracy_order.push_back(u);
        for (int v = 0; v < n; ++v) {
            if (graph[u][v] && !removed[v]) degrees[v]--;
        }
    }

    auto start = chrono::high_resolution_clock::now();
    chiba_clique_listing(graph, degeneracy_order, id_to_original);
    auto end = chrono::high_resolution_clock::now();
    outfile.close();

    cout << "Finished in "
         << chrono::duration_cast<chrono::seconds>(end - start).count()
         << " seconds. Total cliques printed: " << printed_cliques << endl;
     
  for (const auto& [size, freq] : cliqueSizeFreq) {
    distfile << size << " " << freq << endl;
  }

  sizefile.close();
  distfile.close();

  cout << "Size of the largest maximal clique :" << max_clique_size << endl;
  return 0;
}
