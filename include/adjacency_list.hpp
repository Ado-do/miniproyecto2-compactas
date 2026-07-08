#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace mp2 {

/// Undirected simple graph stored as symmetric adjacency lists.
class AdjacencyList {
public:
    AdjacencyList() = default;
    explicit AdjacencyList(std::size_t n);

    std::size_t vertices() const { return adj_.size(); }
    std::size_t edges() const { return edge_count_; }

    void add_edge(std::uint32_t u, std::uint32_t v);
    void build_from_edges(std::size_t n, const std::vector<std::pair<std::uint32_t, std::uint32_t>> &edges);

    __attribute__((noinline)) std::size_t degree(std::uint32_t v) const;
    __attribute__((noinline)) bool neighbors(std::uint32_t u, std::uint32_t v) const;

    const std::vector<std::uint32_t> &adjacent(std::uint32_t v) const;

    /// Total bytes used by the representation (for space benchmarks).
    std::size_t size_bytes() const;

private:
    std::vector<std::vector<std::uint32_t>> adj_;
    std::size_t edge_count_ = 0;
};

} // namespace mp2
