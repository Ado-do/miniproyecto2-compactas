#include "pemb_graph.hpp"

#include <complementary/Graph.hpp>
#include <sdsl/util.hpp>

#include <stdexcept>

namespace mp2 {

PembGraph::PembGraph(std::unique_ptr<Graph> graph, std::unique_ptr<sdsl::pemb<>> pemb)
    : graph_(std::move(graph)), pemb_(std::move(pemb)) {}

PembGraph::~PembGraph() = default;

PembGraph PembGraph::from_pg_graph(const PgGraph &pg) {
    Graph scratch = pg_to_sdsl_graph(pg);
    auto pe = std::make_unique<sdsl::pemb<>>(scratch);
    auto graph = std::make_unique<Graph>(pg_to_sdsl_graph(pg));
    return PembGraph(std::move(graph), std::move(pe));
}

PembGraph PembGraph::from_pg_file(const std::string &path) {
    return from_pg_graph(read_pg_graph(path));
}

PembGraph PembGraph::from_component(const ComponentPgGraph &component) {
    return from_pg_graph(component.pg);
}

std::size_t PembGraph::vertices() const {
    return graph_->vertices();
}

std::size_t PembGraph::edges() const {
    return graph_->edges();
}

std::size_t PembGraph::degree(std::uint32_t v) const {
    if (v >= graph_->vertices()) {
        return 0;
    }
    return static_cast<std::size_t>(graph_->getVertexLast(v) - graph_->getVertexFirst(v) + 1);
}

bool PembGraph::neighbors(std::uint32_t u, std::uint32_t v) const {
    if (u >= graph_->vertices() || v >= graph_->vertices()) {
        return false;
    }
    for (unsigned int e = graph_->getVertexFirst(u); e <= graph_->getVertexLast(u); ++e) {
        if (graph_->getEdgeTgt(e) == v) {
            return true;
        }
    }
    return false;
}

std::size_t PembGraph::size_bytes() const {
    return sdsl::size_in_bytes(*pemb_);
}

std::vector<PembGraph> read_pg_pemb_components(const std::string &path) {
    const PgGraph pg = read_pg_graph(path);
    const auto components = extract_pg_components(pg);

    std::vector<PembGraph> result;
    result.reserve(components.size());
    for (const auto &component : components) {
        result.push_back(PembGraph::from_component(component));
    }
    return result;
}

} // namespace mp2
