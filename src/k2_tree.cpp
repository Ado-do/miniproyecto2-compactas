#include "k2_tree.hpp"

#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <vector>

namespace mp2 {

namespace {

void extract_directed_arcs(const AdjacencyList &graph, std::vector<std::uint32_t> &xedges,
                           std::vector<std::uint32_t> &yedges) {
    xedges.clear();
    yedges.clear();

    const auto n = static_cast<std::uint32_t>(graph.vertices());
    for (std::uint32_t v = 0; v < n; ++v) {
        for (const std::uint32_t neighbor : graph.adjacent(v)) {
            xedges.push_back(v);
            yedges.push_back(neighbor);
        }
    }
}

} // namespace

K2TreeGraph::K2TreeGraph(mp2_k2_tree *tree, std::string basename)
    : tree_(tree), basename_(std::move(basename)) {}

K2TreeGraph::~K2TreeGraph() = default;

std::uint32_t K2TreeGraph::auto_max_level(const std::size_t vertices) {
    if (vertices > UINT32_MAX) {
        throw std::out_of_range("graph too large for k2-tree");
    }
    return mp2_k2_auto_max_level(static_cast<std::uint32_t>(vertices));
}

K2TreeGraph K2TreeGraph::build(const AdjacencyList &graph, const std::string &basename,
                               const K2BuildOptions &options) {
    if (graph.vertices() > UINT32_MAX) {
        throw std::out_of_range("graph too large for k2-tree");
    }

    std::vector<std::uint32_t> xedges;
    std::vector<std::uint32_t> yedges;
    extract_directed_arcs(graph, xedges, yedges);

    const int32_t max_level_override =
        options.max_level.has_value() ? static_cast<int32_t>(*options.max_level) : -1;
    mp2_k2_tree *built = mp2_k2_build_from_arcs_level(
        xedges.data(), yedges.data(), static_cast<std::uint32_t>(graph.vertices()),
        static_cast<std::uint64_t>(xedges.size()), basename.c_str(), max_level_override);
    if (built == nullptr) {
        throw std::runtime_error("failed to build k2-tree: " + basename);
    }

    return K2TreeGraph(built, basename);
}

K2TreeGraph K2TreeGraph::load(const std::string &basename) {
    mp2_k2_tree *loaded = mp2_k2_load(basename.c_str());
    if (loaded == nullptr) {
        throw std::runtime_error("failed to load k2-tree: " + basename);
    }

    return K2TreeGraph(loaded, basename);
}

std::size_t K2TreeGraph::vertices() const {
    return mp2_k2_vertices(tree_.get());
}

std::size_t K2TreeGraph::arcs() const {
    return static_cast<std::size_t>(mp2_k2_arcs(tree_.get()));
}

std::size_t K2TreeGraph::degree(std::uint32_t v) const {
    if (v >= vertices()) {
        throw std::out_of_range("vertex id out of range");
    }

    return mp2_k2_degree(tree_.get(), v);
}

bool K2TreeGraph::neighbors(std::uint32_t u, std::uint32_t v) const {
    if (u >= vertices() || v >= vertices()) {
        return false;
    }

    return mp2_k2_neighbors(tree_.get(), u, v) != 0;
}

std::uint32_t K2TreeGraph::max_level() const {
    return mp2_k2_max_level(tree_.get());
}

std::size_t K2TreeGraph::size_bytes_on_disk() const {
    const std::filesystem::path path = std::filesystem::path(basename_ + ".kt");
    std::error_code ec;
    const auto bytes = std::filesystem::file_size(path, ec);
    if (ec) {
        throw std::runtime_error("cannot stat k2-tree file: " + path.string());
    }
    return static_cast<std::size_t>(bytes);
}

} // namespace mp2
