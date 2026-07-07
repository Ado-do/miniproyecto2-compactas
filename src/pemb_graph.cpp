#include "pemb_graph.hpp"

#include <complementary/Graph.hpp>
#include <sdsl/util.hpp>

#include <stdexcept>
#include <unordered_set>

namespace mp2 {

namespace {

std::unordered_set<std::uint32_t> graph_neighbors(Graph &graph, std::uint32_t vertex) {
    std::unordered_set<std::uint32_t> neighbors;

    if (vertex >= graph.vertices()) {
        return neighbors;
    }

    for (unsigned int edge = graph.getVertexFirst(vertex); edge <= graph.getVertexLast(vertex); ++edge) {
        neighbors.insert(graph.getEdgeTgt(edge));
    }

    for (unsigned int edge = 0; edge < 2 * graph.edges(); ++edge) {
        if (graph.getEdgeTgt(edge) == vertex) {
            neighbors.insert(graph.getEdgeSrc(edge));
        }
    }

    return neighbors;
}

} // namespace

PembGraph::PembGraph(std::unique_ptr<Graph> graph, std::unique_ptr<sdsl::pemb<>> pemb)
    : graph_(std::move(graph)), pemb_(std::move(pemb)) {}

PembGraph::~PembGraph() = default;

PembGraph PembGraph::from_pg_graph(const PgGraph &pg) {
    Graph scratch = pg_to_sdsl_graph(pg);
    auto pemb = std::make_unique<sdsl::pemb<>>(scratch);
    auto graph = std::make_unique<Graph>(pg_to_sdsl_graph(pg));
    return PembGraph(std::move(graph), std::move(pemb));
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
    return graph_neighbors(*const_cast<Graph *>(graph_.get()), v).size();
}

bool PembGraph::neighbors(std::uint32_t u, std::uint32_t v) const {
    if (u >= graph_->vertices() || v >= graph_->vertices()) {
        return false;
    }

    const auto neighbors = graph_neighbors(*const_cast<Graph *>(graph_.get()), u);
    return neighbors.find(v) != neighbors.end();
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
