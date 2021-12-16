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

    void operator()(graph& g, std::string output_path) {
        std::unordered_map<network_vertex, uint32_t> vertices = g.vertices();
        
        std::ofstream out;
        out.open(output_path, std::ios::out | std::ios::binary);

        for (std::pair<network_vertex, uint32_t> v : vertices) {
            if (v.first.is_group) continue;

            std::unordered_map<network_vertex, double> results;
            for(uint32_t iterations = 0 ; iterations < max_it_per_v ; ++iterations)
                random_walk(g, v.first, results);
            
            for (auto& res : results){
                out.write((char*)&v.first.id, sizeof(uint32_t));
                out.write((char*)&res.first.id, sizeof(uint32_t));
                out.write( reinterpret_cast<const char*>( &res.second ), sizeof( double ));
            }       
        }

        out.close();
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
    if (argc != 4){
        fmt::print("Please enter path to file with links as first argument, and path with affiliations as second argument and output dir path as third argument.\n");
        return 0;
    }
        
    std::string links_path = argv[1];
    std::string groups_test = argv[2];
    std::string output_dir = argv[3];
    
    if ( output_dir[output_dir.size() - 1] != '/' ) 
        output_dir.push_back('/');

    std::vector<uint32_t> path_lens{ 3, 5, 10, 15, 20 };
    std::vector<uint32_t> iterations_nums{ 100, 500, 1000, 5000, 10000 };

    graph graph;
    
    read_graph(graph, links_path, false);
    read_graph(graph, groups_test, true);
    
    stopwatch total("total");

    for(auto path_len : path_lens)
        for(auto iterations : iterations_nums){
            stopwatch s(fmt::format("random katz with {} iterations and path len {}", iterations, path_len));
            random_katz rk(iterations, path_len);
            rk(graph, output_dir + fmt::format("recs_{}_{}.bin", path_len, iterations));
            fmt::print("\n");
        }
        
    return 0;
}