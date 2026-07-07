#pragma once

#include "adjacency_list.hpp"
#include "components.hpp"

#include <string>

namespace mp2 {

/// Metadata read from a UdeC planar graph `.pg` file.
struct PgGraph {
    std::size_t n = 0;
    std::size_t m = 0;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> directed_edges;
};

/// Parse n, m and edge list from a `.pg` file (UdeC format).
PgGraph read_pg_graph(const std::string &path);

/// Build an adjacency list, deduplicating undirected edges from directed pairs.
AdjacencyList pg_to_adjacency_list(const PgGraph &pg);

/// Read a `.pg` file and return one adjacency list per connected component.
std::vector<ComponentGraph> read_pg_components(const std::string &path);

} // namespace mp2
