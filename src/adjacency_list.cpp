#include "adjacency_list.hpp"

#include <algorithm>
#include <stdexcept>

namespace mp2 {

AdjacencyList::AdjacencyList(std::size_t n) : adj_(n) {}

void AdjacencyList::add_edge(std::uint32_t u, std::uint32_t v) {
    if (u >= adj_.size() || v >= adj_.size()) {
        throw std::out_of_range("vertex id out of range");
    }
    if (u == v) {
        return;
    }
    const std::size_t before = adj_[u].size();
    auto insert_unique = [](std::vector<std::uint32_t> &list, std::uint32_t x) {
        if (std::find(list.begin(), list.end(), x) == list.end()) {
            list.push_back(x);
        }
    };
    insert_unique(adj_[u], v);
    insert_unique(adj_[v], u);
    if (adj_[u].size() > before) {
        ++edge_count_;
    }
}

void AdjacencyList::build_from_edges(std::size_t n,
                                     const std::vector<std::pair<std::uint32_t, std::uint32_t>> &edges) {
    adj_.assign(n, {});
    edge_count_ = 0;
    for (const auto &[u, v] : edges) {
        if (u >= n || v >= n) {
            throw std::out_of_range("edge references invalid vertex");
        }
        add_edge(u, v);
    }
}

std::size_t AdjacencyList::degree(std::uint32_t v) const {
    if (v >= adj_.size()) {
        throw std::out_of_range("vertex id out of range");
    }
    return adj_[v].size();
}

bool AdjacencyList::neighbors(std::uint32_t u, std::uint32_t v) const {
    if (u >= adj_.size() || v >= adj_.size()) {
        return false;
    }
    const auto &list = adj_[u];
    return std::find(list.begin(), list.end(), v) != list.end();
}

const std::vector<std::uint32_t> &AdjacencyList::adjacent(std::uint32_t v) const {
    if (v >= adj_.size()) {
        throw std::out_of_range("vertex id out of range");
    }
    return adj_[v];
}

std::size_t AdjacencyList::size_bytes() const {
    std::size_t bytes = sizeof(*this) + adj_.size() * sizeof(std::vector<std::uint32_t>);
    for (const auto &list : adj_) {
        bytes += list.size() * sizeof(std::uint32_t);
    }
    return bytes;
}

} // namespace mp2
