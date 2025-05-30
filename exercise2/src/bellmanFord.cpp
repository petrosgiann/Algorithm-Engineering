#include "bellmanFord.hpp"
#include "visitor_graph.hpp"
#include <queue>


void Update_pred(const Graph& G, Vertex v,
                 const std::vector<bool>& in_R,
                 std::vector<bool>& reached,
                 std::vector<Edge>& pred);


using namespace boost;
bool bellmanFord(Graph& G, Vertex s, std::vector<long>& dist, std::vector<Edge>& pred) {
    const Edge edge_nil = *(edges(G).second);
    const Vertex vertex_nil = boost::graph_traits<Graph>::null_vertex();

    std::queue<Vertex> Q;
    std::vector<bool> in_Q(num_vertices(G), false);

    for (std::size_t i = 0; i < num_vertices(G); ++i) {
        pred[i] = edge_nil;
    }

    dist[s] = 0;
    Q.push(s);
    Q.push(vertex_nil);
    in_Q[s] = true;

    std::size_t n = num_vertices(G);
    int phase_count = 0;

    while (phase_count < n) {
        Vertex u = Q.front(); Q.pop();

        if (u == vertex_nil) {
            phase_count++;
            if (Q.empty()) return true; // Δεν υπάρχει αρνητικός κύκλος
            Q.push(vertex_nil);
            continue;
        }

        in_Q[u] = false;

        OutEdgeIterator ei, ei_end;
        for (boost::tie(ei, ei_end) = out_edges(u, G); ei != ei_end; ++ei) {
            Vertex v = target(*ei, G);
            long weight = G[*ei].weight;

            if ((pred[v] == edge_nil && v != s) || dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pred[v] = *ei;

                if (!in_Q[v]) {
                    Q.push(v);
                    in_Q[v] = true;
                }
            }
        }
    }


    
    
    if (pred[s] != edge_nil) return false;

    std::vector<bool> in_R(num_vertices(G), false);  
    
    EdgeIterator ed_first,ed_second;
    std::vector<Edge> removed_vertices;
     for (boost::tie(ed_first ,ed_second) = edges(G); ed_first != ed_second; ++ed_first) {
            if(*ed_first != pred[target(*ed_first,G)])
            {
                removed_vertices.push_back(*ed_first);
                
            }
        }

        for(int i=0; i< removed_vertices.size();i++)
        {
            remove_edge(removed_vertices[i],G);
        }

        my_dfs_visitor vis(G);


    
    boost::depth_first_search(G,visitor(vis));
    in_R = vis.return_vector();

    
    std::vector<bool> reached_from_node_in_U(num_vertices(G), false);
    
    for (Vertex v = 0; v < num_vertices(G); ++v) {
        if (in_Q[v] && !reached_from_node_in_U[v]) {
            Update_pred(G, v, in_R, reached_from_node_in_U, pred);
        }
    }

    

    return false; // Υπάρχει αρνητικός κύκλος
}




void Update_pred(const Graph& G, Vertex v,
                 const std::vector<bool>& in_R,
                 std::vector<bool>& reached,
                 std::vector<Edge>& pred)
{
    reached[v] = true;

    OutEdgeIterator ei, ei_end;
    for (boost::tie(ei, ei_end) = out_edges(v, G); ei != ei_end; ++ei) {
        Vertex w = target(*ei, G);
        if (!reached[w]) {
            if (in_R[w])
                pred[w] = *ei;
            Update_pred(G, w, in_R, reached, pred);
        }
    }
}




