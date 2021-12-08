#include <vector>
#include <inttypes.h>
#include <utility>
#include <exception>
#include <unordered_map>

struct network_vertex {
    uint32_t id;
    bool is_group;
    network_vertex(uint32_t id = -1, bool is_group = false) : id(id), is_group(is_group) {}
    friend bool operator==(const network_vertex& v, const network_vertex& w){ return w.id == v.id & w.is_group == v.is_group; }
};

namespace std {
  template <>
  struct hash<network_vertex>
  {
    std::size_t operator()(const network_vertex& k) const
    {
      using std::hash;

      return hash<uint32_t>()(k.id);
    }
  };
}

// graph is undirected, weightless
class graph {
    std::vector<std::pair<std::vector<uint32_t>, network_vertex>> g;
    std::unordered_map<network_vertex, uint32_t> vertex_index_map;
    uint32_t edges_num = 0;

    bool vertex_in_graph(network_vertex& v) {
        return vertex_index_map.find(v) != vertex_index_map.end();
    }

    uint32_t get_vertex_id(network_vertex& v) {     // if to slow write mapping between network vertex and index with unordered map
        if (!vertex_in_graph(v))
            throw std::runtime_error("Vertex not in graph.");
        
        return vertex_index_map[v];
    }

public:
    graph() { }

    uint32_t num_vertices() { return g.size(); }
    uint32_t num_edges() { return edges_num; }

    bool add_vertex(network_vertex& v) {
        if(vertex_in_graph(v)) return false;
        g.emplace_back(std::vector<uint32_t>(), v);
        vertex_index_map[v] = g.size() - 1;
        return true;
    }

    bool add_edge(network_vertex& v, network_vertex& w) {
        auto ind_v = get_vertex_id(v);
        auto ind_w = get_vertex_id(w);

        if (std::find(g[ind_v].first.begin(), g[ind_v].first.end(), ind_w) != g[ind_v].first.end())
            return false;

        if (std::find(g[ind_w].first.begin(), g[ind_w].first.end(), ind_v) != g[ind_w].first.end())
            throw std::runtime_error("Graph isn't symetric.");

        g[ind_v].first.emplace_back(ind_w);
        g[ind_w].first.emplace_back(ind_v);
        edges_num++;
        return true;
    }

    uint32_t degree(network_vertex& v) {
        return g[vertex_index_map[v]].first.size();
    }

    network_vertex get_neigh(network_vertex& v, uint32_t index) {
        return g[g[get_vertex_id(v)].first[index]].second;       // prvo dohvacamo listu susjeda od v te index i-tog susjeda, zatim vracamo vrh koji se nalazi na i-tom mjestu u grafu
    }

    std::unordered_map<network_vertex, uint32_t> vertices() { return vertex_index_map;}

    // std::vector<network_vertex> get_neigh_groups(network_vertex& v) {
    //     std::vector<network_vertex> groups;

    //     for(auto neigh : g[get_vertex_id(v)].first )
    //         if (g[neigh].second.is_group)
    //             groups.push_back(g[neigh].second);

    //     return groups;
    // }
};