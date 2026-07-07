#pragma once

#include "adjacency_list.hpp"
#include "k2_tree_bridge.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace mp2 {

/// RAII wrapper around k2-tree Basic with a unified graph API.
class K2TreeGraph {
public:
    static K2TreeGraph build(const AdjacencyList &graph, const std::string &basename);
    static K2TreeGraph load(const std::string &basename);

    K2TreeGraph(const K2TreeGraph &) = delete;
    K2TreeGraph &operator=(const K2TreeGraph &) = delete;
    K2TreeGraph(K2TreeGraph &&) noexcept = default;
    K2TreeGraph &operator=(K2TreeGraph &&) noexcept = default;
    ~K2TreeGraph();

    std::size_t vertices() const;
    std::size_t arcs() const;
    std::size_t degree(std::uint32_t v) const;
    bool neighbors(std::uint32_t u, std::uint32_t v) const;

    const std::string &basename() const { return basename_; }
    std::size_t size_bytes_on_disk() const;

private:
    struct TreeDeleter {
        void operator()(mp2_k2_tree *tree) const { mp2_k2_destroy(tree); }
    };

    explicit K2TreeGraph(mp2_k2_tree *tree, std::string basename);

    std::unique_ptr<mp2_k2_tree, TreeDeleter> tree_;
    std::string basename_;
};

} // namespace mp2
