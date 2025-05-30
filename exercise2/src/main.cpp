#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include <LEDA/graph/graph.h>
#include <LEDA/graph/edge_array.h>
#include <LEDA/core/random_source.h>
#include <LEDA/system/timer.h>
#include <LEDA/graph/templates/shortest_path.h>


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>

#include "bellmanFord.hpp"
#include "visitor_graph.hpp"


typedef leda::graph LedaGraph;
typedef leda::edge LedaEdge;
typedef leda::edge_array<int> LedaEdgeArray;


void LEDA_Graph_To_Boost(leda::graph& L_G, leda::edge_array<int>& wt, Graph& B_G) 
{
    leda::node_array<Vertex> Map(L_G);

    leda::edge e;
    leda::node n;

    forall_nodes(n, L_G)
    {
        Vertex v = add_vertex(B_G);
        Map[n] = v;
    }

    forall_edges(e, L_G)
    {
        Edge u = add_edge(Map[L_G.source(e)], Map[L_G.target(e)], B_G).first;
        B_G[u].weight = wt[e];
    }
}

void evaluate_randomgraph_experiment(int n) {
    int m = int(20 * n * std::log(double(n)));

    std::cout << "\n--- Benchmark για n = " << n << ", m = " << m << " ---\n";

    // 1. LEDA γράφος
    LedaGraph L_G;
    leda::random_graph(L_G, n, m, true, true, true);
    leda::Make_Connected(L_G);

    LedaEdgeArray wt(L_G);
    leda::random_source S(-100, 10000);
    LedaEdge e;
    forall_edges(e, L_G) S >> wt[e];

    // 2. Boost γράφος (αντίγραφο του LEDA)
    Graph B_G;
    LEDA_Graph_To_Boost(L_G, wt, B_G);

    // 3. Επιλογή τυχαίας κορυφής s
    std::srand(std::time(0));
    int s = std::rand() % n;
    std::cout << "Αρχική κορυφή s = " << s << std::endl;

    // 4. Εκτέλεση της δικής σου bellmanFord
    std::vector<long> dist(n, std::numeric_limits<long>::max());
    std::vector<Edge> pred(n);

    leda::timer T1;
    T1.start();
    bool has_neg_cycle = !bellmanFord(B_G, s, dist, pred);
    T1.stop();

    std::cout << "Χρόνος δικής μου υλοποίησης: " << T1.elapsed_time() << " sec\n";
    std::cout << "Αρνητικός κύκλος: " << (has_neg_cycle ? "ΝΑΙ" : "ΟΧΙ") << "\n";

    // 5. Εκτέλεση LEDA Bellman-Ford
    leda::node_array<int> leda_dist(L_G);
    leda::node_array<leda::edge> leda_pred(L_G);

    // Αντιστοίχιση s -> leda node
    leda::node leda_s = L_G.first_node();
    int count = 0;
    for (leda::node v = L_G.first_node(); v != nil; v = L_G.succ_node(v)) {
        if (count == s) {
            leda_s = v;
            break;
        }
        count++;
    }

    leda::timer T2;
    T2.start();
    bool leda_has_neg = !leda::BELLMAN_FORD_B_T(L_G, leda_s, wt, leda_dist, leda_pred);
    T2.stop();

    std::cout << "Χρόνος LEDA Bellman-Ford: " << T2.elapsed_time() << " sec\n";
    std::cout << "LEDA αρνητικός κύκλος: " << (leda_has_neg ? "ΝΑΙ" : "ΟΧΙ") << "\n";

    if (!has_neg_cycle) {
    std::vector<long> boost_dist(n, std::numeric_limits<long>::max());
    std::vector<Vertex> boost_pred(n);
    boost::property_map<Graph, boost::vertex_index_t>::type index_map = get(boost::vertex_index, B_G);
    leda::timer T3;
    T3.start();
    bool boost_success = boost::bellman_ford_shortest_paths(B_G, n,
        boost::weight_map(get(&myEdge::weight, B_G))
        .distance_map(&boost_dist[0])
        .predecessor_map(&boost_pred[0])
        .vertex_index_map(index_map));
    T3.stop();

    std::cout << "Χρόνος Boost bellman_ford_shortest_paths(): " << T3.elapsed_time() << " sec\n";
    std::cout << "Boost αρνητικός κύκλος: " << (!boost_success ? "ΝΑΙ" : "ΟΧΙ") << "\n";
} else {
    std::cout << "[SKIP] Boost δεν εκτελείται λόγω αρνητικού κύκλου στη δική υλοποίηση.\n";
}
}



void create_grid_graph(leda::graph& G, std::vector<std::vector<leda::node> >& grid, int n) {
    grid.resize(n, std::vector<leda::node>(n));

    // === Βήμα 1: Δημιουργία κόμβων ===
    for (int x = 0; x < n; ++x)
        for (int y = 0; y < n; ++y)
            grid[x][y] = G.new_node();

    // === Βήμα 2: Προσθήκη ακμών ανά περιοχή ===
    for (int x = 0; x < n; ++x) {
        for (int y = 0; y < n; ++y) {
            // Περιοχή 1
            if (x < n / 2) {
                if (x + 1 < n) G.new_edge(grid[x][y], grid[x + 1][y]);
                if (y + 1 < n) G.new_edge(grid[x][y], grid[x][y + 1]);
            }

            // Περιοχή 2
            if (x >= n / 2 && y >= n / 2) {
                if (x + 1 < n) G.new_edge(grid[x][y], grid[x + 1][y]);
                if (y + 1 < n) G.new_edge(grid[x][y], grid[x][y + 1]);
            }

            // Περιοχή 3
            if (x >= n / 2 && y < n / 2) {
                if (x + 1 < n) {
                    if (rand() % 2 == 0)
                        G.new_edge(grid[x][y], grid[x + 1][y]);
                    else
                        G.new_edge(grid[x + 1][y], grid[x][y]);
                }
                if (y + 1 < n) {
                    if (rand() % 2 == 0)
                        G.new_edge(grid[x][y], grid[x][y + 1]);
                    else
                        G.new_edge(grid[x][y + 1], grid[x][y]);
                }
            }
        }
    }

    // === Βήμα 3: Προσθήκη 2 ειδικών ακμών με κόστος -100000 ===
    int mid = n / 2;
    if (mid + 1 < n && mid - 1 >= 0) {
        G.new_edge(grid[mid + 1][mid], grid[mid + 1][mid - 1]);
        G.new_edge(grid[mid + 1][mid - 1], grid[mid][mid - 1]);
    }
}

void assign_random_weights(leda::graph& G, leda::edge_array<int>& weights, const std::vector<std::vector<leda::node> >& grid, int n) {
    std::srand(std::time(0));
    for (leda::edge e = G.first_edge(); e != nil; e = G.succ_edge(e)) {
        leda::node src = G.source(e);
        leda::node tgt = G.target(e);

        int src_x = -1, src_y = -1, tgt_x = -1, tgt_y = -1;

        for (int x = 0; x < n && (src_x == -1 || tgt_x == -1); ++x) {
            for (int y = 0; y < n && (src_y == -1 || tgt_y == -1); ++y) {
                if (grid[x][y] == src) {
                    src_x = x; src_y = y;
                }
                if (grid[x][y] == tgt) {
                    tgt_x = x; tgt_y = y;
                }
            }
        }

        // Κατηγοριοποίηση περιοχής
        if (src_x < n / 2) {
            weights[e] = rand() % 10001;
        } else if (src_x >= n / 2 && src_y >= n / 2) {
            weights[e] = rand() % 10001;
        } else if (src_x >= n / 2 && src_y < n / 2) {
            weights[e] = -100 + rand() % 10100;
        }

        // Ειδικές ακμές (τις αναγνωρίζουμε με τις θέσεις)
        if ((src_x == n / 2 + 1 && src_y == n / 2 && tgt_x == n / 2 + 1 && tgt_y == n / 2 - 1) ||
            (src_x == n / 2 + 1 && src_y == n / 2 - 1 && tgt_x == n / 2 && tgt_y == n / 2 - 1)) {
            weights[e] = -100000;
        }
    }
}

void evaluate_grid_experiment(int n) {
    std::cout << "\n--- Grid Benchmark για n = " << n << " ---\n";

    leda::graph G;
    std::vector<std::vector<leda::node> > grid;
    create_grid_graph(G, grid, n);

    leda::edge_array<int> weights(G);
    assign_random_weights(G, weights, grid, n);

    leda::node s = grid[0][0];
    std::cout << "Αρχική κορυφή s = (0,0)\n";

    Graph boost_graph;
    LEDA_Graph_To_Boost(G, weights, boost_graph);

    size_t V = num_vertices(boost_graph);
    std::vector<long> my_dist(V, std::numeric_limits<long>::max());
    std::vector<Edge> my_pred(V);

    leda::timer T1;
    T1.start();
    bool has_neg_cycle = !bellmanFord(boost_graph, 0, my_dist, my_pred);
    T1.stop();

    std::cout << "Χρόνος δικής μου υλοποίησης: " << T1.elapsed_time() << " sec\n";
    std::cout << "Αρνητικός κύκλος: " << (has_neg_cycle ? "ΝΑΙ" : "ΟΧΙ") << "\n";

    leda::node_array<int> dist(G);
    leda::node_array<leda::edge> pred(G);

    leda::timer T2;
    T2.start();
    bool leda_neg = !leda::BELLMAN_FORD_B_T(G, s, weights, dist, pred);
    T2.stop();

    std::cout << "Χρόνος LEDA Bellman-Ford: " << T2.elapsed_time() << " sec\n";
    std::cout << "LEDA αρνητικός κύκλος: " << (leda_neg ? "ΝΑΙ" : "ΟΧΙ") << "\n";

    if (!has_neg_cycle) {
        std::vector<long> boost_dist(V, std::numeric_limits<long>::max());
        std::vector<Vertex> boost_pred(V);
        leda::timer T3;
        T3.start();
        bool boost_result = boost::bellman_ford_shortest_paths(
            boost_graph, int(V),
            boost::weight_map(get(&myEdge::weight, boost_graph))
                .distance_map(&boost_dist[0])
                .predecessor_map(&boost_pred[0])
        );
        T3.stop();
        std::cout << "Χρόνος Boost bellman_ford_shortest_paths(): " << T3.elapsed_time() << " sec\n";
        std::cout << "Boost αρνητικός κύκλος: " << (!boost_result ? "ΝΑΙ" : "ΟΧΙ") << "\n";
    } else {
        std::cout << "Παράκαμψη Boost: αρνητικός κύκλος.\n";
    }

    std::cout << "Πρώτες 10 αποστάσεις από s:\n";
    int count = 0;
    for (leda::node v = G.first_node(); v != nil && count < 10; v = G.succ_node(v), ++count) {
        int d = dist[v];
        if (d >= std::numeric_limits<int>::max() / 2)
            std::cout << "dist[" << count << "] = ∞\n";
        else
            std::cout << "dist[" << count << "] = " << d << "\n";
    }
}


int main() {

    int sizes[3] = {100, 200, 300};
    for (int i = 0; i < 3; ++i) {
        int n = sizes[i];
        for (int run = 1; run <= 5; ++run) {
            std::cout << "\n[RUN " << run << "]\n";
            evaluate_grid_experiment(n);
        }
    }

  //



    return 0;
}