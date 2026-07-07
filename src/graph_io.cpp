#include "graph_io.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace mp2 {

namespace {

std::pair<std::uint32_t, std::uint32_t> edge_key(std::uint32_t u, std::uint32_t v) {
    if (u > v) {
        std::swap(u, v);
    }
    return {u, v};
}

} // namespace

PgGraph read_pg_graph(const std::string &path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("cannot open graph file: " + path);
    }

    PgGraph pg;
    if (!(in >> pg.n >> pg.m)) {
        throw std::runtime_error("invalid graph header: " + path);
    }

    pg.directed_edges.reserve(pg.m * 2);
    for (std::size_t i = 0; i < pg.m * 2; ++i) {
        std::uint32_t u = 0;
        std::uint32_t v = 0;
        if (!(in >> u >> v)) {
            throw std::runtime_error("unexpected end of graph file: " + path);
        }
        pg.directed_edges.emplace_back(u, v);
    }

    return pg;
}

AdjacencyList pg_to_adjacency_list(const PgGraph &pg) {
    std::vector<std::pair<std::uint32_t, std::uint32_t>> unique_edges;
    unique_edges.reserve(pg.m);
    std::unordered_set<std::uint64_t> seen;
    seen.reserve(pg.m * 2);

    for (const auto &[u, v] : pg.directed_edges) {
        const auto [a, b] = edge_key(u, v);
        const std::uint64_t key = (static_cast<std::uint64_t>(a) << 32) | b;
        if (seen.insert(key).second) {
            unique_edges.emplace_back(a, b);
        }
    }

    AdjacencyList graph(pg.n);
    graph.build_from_edges(pg.n, unique_edges);
    return graph;
}

std::vector<ComponentGraph> read_pg_components(const std::string &path) {
    const PgGraph pg = read_pg_graph(path);
    const AdjacencyList graph = pg_to_adjacency_list(pg);
    return extract_components(graph);
}

} // namespace mp2
