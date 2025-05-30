#include <iostream>
#include <cmath>
#include <LEDA/graph/graph.h>
#include <LEDA/graph/edge_array.h>
#include <LEDA/graph/graph_misc.h>
#include <LEDA/system/timer.h>
#include <LEDA/graph/basic_graph_alg.h>
#include <LEDA/core/random_source.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/list.h>
#include <LEDA/core/stack.h>
#include <vector>

#include "kruskal.hpp"  // η δικιά σου υλοποίηση

using namespace leda;

// Έλεγχος εγκυρότητας MST
bool checker(graph &G, edge_array<bool>& TreeEdges, edge_array<int> wt) {
    edge e;
    node n;

    node_array<int> dist(G, -1);
    node_array<edge> pred(G);

    forall_edges(e, G) {
        if (!TreeEdges[e]) {
            G.hide_edge(e);
        }
    }

    list<edge> h_edges = G.hidden_edges();

    node root = G.first_node();
    BFS(G, root, dist, pred);

    forall(e, h_edges) {
        node u = G.source(e);
        node v = G.target(e);
        if (dist[u] == -1 || dist[v] == -1)
            continue;

        while (dist[u] != dist[v]) {
            if (dist[u] > dist[v]) {
                if (pred[u] == nil || wt[pred[u]] > wt[e])
                    return false;
                u = G.opposite(u, pred[u]);
            } else {
                if (pred[v] == nil || wt[pred[v]] > wt[e])
                    return false;
                v = G.opposite(v, pred[v]);
            }
        }

        while (u != v) {
            if (pred[u] == nil || wt[pred[u]] > wt[e]) return false;
            if (pred[v] == nil || wt[pred[v]] > wt[e]) return false;
            u = G.opposite(u, pred[u]);
            v = G.opposite(v, pred[v]);
        }
    }

    G.restore_all_edges();
    return true;
}

void evaluate_grid_experiment(int r, int c) {
    int n = r * c;
    int m = 2 * r * c - r - c;

    std::cout << "\n--- Benchmark για πλέγμα " << r << " x " << c << " ---\n";

    graph G;
    std::vector<std::vector<node> > nodes(r, std::vector<node>(c));

    for (int i = 0; i < r; ++i)
        for (int j = 0; j < c; ++j)
            nodes[i][j] = G.new_node();

    for (int i = 0; i < r; ++i)
        for (int j = 0; j < c; ++j) {
            if (i < r - 1)
                G.new_edge(nodes[i][j], nodes[i + 1][j]);
            if (j < c - 1)
                G.new_edge(nodes[i][j], nodes[i][j + 1]);
        }

    G.make_undirected();

    edge_array<int> wt(G);
    random_source S(10, 10000);
    edge e;
    forall_edges(e, G) S >> wt[e];

    edge_array<bool> TreeEdges1(G, false);
    timer T1;
    T1.start();
    Kruskal(G, wt, TreeEdges1);
    T1.stop();
    double my_time = T1.elapsed_time();

    int mst_weight_sum = 0, no_mst_weight_sum = 0;
    forall_edges(e, G) {
        if (TreeEdges1[e]) mst_weight_sum += wt[e];
        else no_mst_weight_sum += wt[e];
    }

    std::cout << "Άθροισμα βαρών MST: " << mst_weight_sum << "\n";
    std::cout << "Άθροισμα βαρών εκτός MST: " << no_mst_weight_sum << "\n";

    timer T2;
    T2.start();
    leda::MIN_SPANNING_TREE(G, wt);
    T2.stop();
    double leda_time = T2.elapsed_time();

    std::cout << "Χρόνος Kruskal (δική μου υλοποίηση): " << my_time << " sec\n";
    std::cout << "Χρόνος Kruskal (LEDA): " << leda_time << " sec\n";

    bool check = checker(G, TreeEdges1, wt);
    std::cout << "Ο έλεγχος πιστοποίησης: " << (check ? "Επιτυχής" : "Απέτυχε") << "\n";
}

void evaluate_experiment(int n) {
    int m = int(2 * n * (std::log(n) / std::log(2)));

    std::cout << "\n--- Benchmark για n = " << n << ", m = " << m << " ---\n";

    graph G;
    random_graph(G, n, m, true, true, true);
    Make_Connected(G);
    G.make_undirected();

    edge_array<int> wt(G);
    random_source S(10, 10000);
    edge e;
    forall_edges(e, G) S >> wt[e];

    edge_array<bool> TreeEdges1(G, false);
    timer T1;
    T1.start();
    Kruskal(G, wt, TreeEdges1);
    T1.stop();
    double my_time = T1.elapsed_time();

    long long mst_weight_sum = 0, no_mst_weight_sum = 0;
    forall_edges(e, G) {
        if (TreeEdges1[e]) mst_weight_sum += wt[e];
        else no_mst_weight_sum += wt[e];
    }

    std::cout << "Άθροισμα βαρών MST: " << mst_weight_sum << "\n";
    std::cout << "Άθροισμα βαρών εκτός MST: " << no_mst_weight_sum << "\n";

    timer T2;
    T2.start();
    leda::MIN_SPANNING_TREE(G, wt);
    T2.stop();
    double leda_time = T2.elapsed_time();

    std::cout << "Χρόνος Kruskal (δική μου υλοποίηση): " << my_time << " sec\n";
    std::cout << "Χρόνος Kruskal (LEDA): " << leda_time << " sec\n";

    bool check = checker(G, TreeEdges1, wt);
    std::cout << "Ο έλεγχος πιστοποίησης: " << (check ? "Επιτυχής" : "Απέτυχε") << "\n";
}


void generate_worst_case_graph(graph& G, int n) {
    std::vector<node> nodes(n);
    for (int i = 0; i < n; ++i)
        nodes[i] = G.new_node();

    for (int i = 0; i < n - 1; ++i)
        G.new_edge(nodes[i], nodes[i + 1]); // Αλυσίδα

    for (int i = 1; i < n; ++i)
        G.new_edge(nodes[0], nodes[i]); // Αστέρι

    G.make_undirected();
}


void evaluate_worst_case_experiment(int n) {
    std::cout << "\n--- Benchmark  worst-case για n = " << n << " ---\n";

    graph G;
    generate_worst_case_graph(G, n);  

    edge_array<int> wt(G); 


    edge e;
    forall_edges(e, G) {
        node u = G.source(e);
        node v = G.target(e);
        if ((u == G.first_node() || v == G.first_node()) && u != v) {
            wt[e] = 30000; // Αστέρι
        } else {
            wt[e] = 30; // Αλυσίδα
        }
    }

    edge_array<bool> TreeEdges(G, false);
    timer T1;
    T1.start();
    Kruskal(G, wt, TreeEdges);
    T1.stop();
    double my_time = T1.elapsed_time();

    long long mst_weight = 0, no_mst_weight = 0;
    forall_edges(e, G)
        if (TreeEdges[e]) mst_weight += wt[e];
        else no_mst_weight += wt[e];

    std::cout << "Άθροισμα βαρών MST: " << mst_weight << "\n";
    std::cout << "Άθροισμα βαρών εκτός MST: " << no_mst_weight << "\n";
    std::cout << "Χρόνος Kruskal (δική μου υλοποίηση): " << my_time << " sec\n";

    bool check = checker(G, TreeEdges, wt);
    std::cout << "Ο έλεγχος πιστοποίησης: " << (check ? "Επιτυχής" : "Απέτυχε") << "\n";
}




int main() {
    evaluate_experiment(4000);
    evaluate_experiment(8000);
    evaluate_experiment(16000);

    evaluate_grid_experiment(100, 100);
    evaluate_grid_experiment(200, 200);
    evaluate_grid_experiment(300, 300);

    evaluate_worst_case_experiment(2000);
    evaluate_worst_case_experiment(4000);



    return 0;
}
