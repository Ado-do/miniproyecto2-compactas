#pragma once

#include "adjacency_list.hpp"
#include "components.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace mp2 {

/// Options for writing the k2-tree adjacency binary format.
struct K2BinaryWriteOptions {
    /// Neighbor ids are written in ascending order per vertex.
    bool sort_neighbors = true;
};

/// Write adjacency in k2-tree binary format:
/// `<n:uint32> <m:uint64> -1 nbrs... -2 nbrs...` with 1-based neighbor ids.
void write_k2_binary(const AdjacencyList &graph, const std::string &path,
                     const K2BinaryWriteOptions &opts = {});

/// Read a k2-tree adjacency binary file into an adjacency list.
AdjacencyList read_k2_binary(const std::string &path);

/// Convenience wrapper for a connected component subgraph.
void write_k2_binary(const ComponentGraph &component, const std::string &path,
                     const K2BinaryWriteOptions &opts = {});

/// Read a `.pg` file, split by connected component, and write one binary per component.
/// Returns the list of generated file paths (`out_dir/basename_c{i}.k2g`).
std::vector<std::string> write_pg_components_k2_binaries(const std::string &pg_path,
                                                         const std::string &out_dir,
                                                         const std::string &basename);

} // namespace mp2
