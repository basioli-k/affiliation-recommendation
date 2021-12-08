#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <inttypes.h>
#include <string>
#include <unordered_map>
#include <math.h>
#include <random>
#include "graph.h"
#include "stopwatch.h"

class random_katz{
    std::random_device rd;
    std::mt19937 gen;
    uint32_t max_it_per_v;
    uint32_t longest_path;
public:
    random_katz(uint32_t mipv = 1000, uint32_t lp = 20) : max_it_per_v(mipv), longest_path(lp) { gen = std::mt19937((rd())); }

    network_vertex get_random_neigh(graph& g, network_vertex v) {
        std::uniform_int_distribution<> distrib(0, g.degree(v) - 1);
        return g.get_neigh(v, distrib(gen));
    }

    void random_walk(graph& g, network_vertex v, std::unordered_map<network_vertex, double>& results) {
        auto curr = v;
        uint32_t path_len = 0;

        while (path_len++ <= longest_path) {
            if (curr.is_group) {
                auto it = results.find(curr);
                if (it == results.end())
                    results[curr] = 0;
                results[curr]+= 1.0 / path_len;
            }   
            curr = get_random_neigh(g, curr);
        }
    }

    void operator()(graph& g) {
        std::unordered_map<network_vertex, uint32_t> vertices = g.vertices();
        
        for (std::pair<network_vertex, uint32_t> v : vertices) {
            if (v.first.is_group) continue;

            std::unordered_map<network_vertex, double> results;
            for(uint32_t iterations = 0 ; iterations < max_it_per_v ; ++iterations)
                random_walk(g, v.first, results);
            
            fmt::print("User {} groups are:\n", v.first.id);
            // do something with these results
            for (auto& res : results)
                fmt::print("Group {} has score {}\n", res.first.id, res.second);
        }
    }

};

void read_graph(graph& graph, std::string& file, bool is_group) {
    std::ifstream in(file);
    uint32_t a,b;
    while(in >> a >> b) {
        network_vertex node1(a, false);
        network_vertex node2(b, is_group);
        graph.add_vertex(node1);
        graph.add_vertex(node2);
        graph.add_edge(node1, node2);
    }
    
    in.close();
}


int main(int argc, char **argv) {
    if (argc != 3){
        fmt::print("Please enter path to file with links as first argument, and path with affiliations as second argument.\n");
        return 0;
    }
        
    std::string links_path = argv[1];
    std::string groups_test = argv[2];

    graph graph;
    
    read_graph(graph, links_path, false);
    read_graph(graph, groups_test, true);
    
    stopwatch s("random katz");
    random_katz rk;
    rk(graph);
    fmt::print("\n");
    return 0;
}