#pragma once

#include "graph_io.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Graph;

namespace mp2 {

/// One connected component with planar embedding edges renumbered to [0, n').
struct ComponentPgGraph {
    PgGraph pg;
    std::vector<std::uint32_t> local_to_global;
};

/// Build an sdsl `Graph` from a UdeC `.pg` structure (preserves CCW edge order).
Graph pg_to_sdsl_graph(const PgGraph &pg);

/// Split a planar graph into connected components, preserving directed edge order.
std::vector<ComponentPgGraph> extract_pg_components(const PgGraph &pg);

/// Convenience: read `.pg` and convert to sdsl `Graph`.
Graph read_sdsl_graph(const std::string &path);

} // namespace mp2
