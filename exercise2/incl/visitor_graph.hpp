#ifndef VIS
#define VIS

#include "bellmanFord.hpp"
#include <boost/graph/depth_first_search.hpp>
#include <boost/shared_ptr.hpp>
#include <LEDA/graph/graph.h>
#include <vector>

using namespace boost;

class my_dfs_visitor : public default_dfs_visitor 
{
public:
    my_dfs_visitor(Graph& g) 
    {
        in_R = boost::shared_ptr<std::vector<bool> >(new std::vector<bool>(num_vertices(g), false));
    }

    void discover_vertex(Vertex u, const Graph&) 
    {
        (*in_R)[u] = true;
    }

    std::vector<bool> return_vector() const 
    {
        return *in_R;
    }

private:
    boost::shared_ptr<std::vector<bool> > in_R;
};


#endif