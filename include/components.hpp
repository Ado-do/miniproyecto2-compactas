#pragma once

#include "adjacency_list.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace mp2 {

/// One connected component with vertices renumbered to [0, n').
struct ComponentGraph {
    AdjacencyList graph;
    /// local id -> original vertex id in the parent graph
    std::vector<std::uint32_t> local_to_global;
};

/// Component id per vertex (same length as vertex count).
std::vector<std::uint32_t> component_labels(const AdjacencyList &graph);

/// Number of connected components (labels must be contiguous from 0).
std::size_t component_count(const std::vector<std::uint32_t> &labels);

/// Split into connected components; each stores a renumbered subgraph.
std::vector<ComponentGraph> extract_components(const AdjacencyList &graph);

} // namespace mp2
