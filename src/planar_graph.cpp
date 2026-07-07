#include "planar_graph.hpp"

#include "components.hpp"

#include <complementary/Graph.hpp>
#include <stdexcept>
#include <unordered_set>

namespace mp2 {

namespace {

void link_reverse_edges(Graph &graph, std::size_t arc_count) {
    for (std::size_t i = 0; i < arc_count; ++i) {
        if (graph.getEdgeCmp(static_cast<int>(i)) != -1) {
            continue;
        }

        const unsigned int target_vertex = graph.getEdgeTgt(static_cast<int>(i));
        Vertex target = graph.getVertex(target_vertex);
        int cmp = -1;

        for (unsigned int j = target.getFirst(); j <= target.getLast(); ++j) {
            if (graph.getEdgeCmp(static_cast<int>(j)) == -1 &&
                graph.getEdgeTgt(static_cast<int>(j)) == graph.getEdgeSrc(static_cast<int>(i)) &&
                static_cast<int>(i) != static_cast<int>(j)) {
                cmp = static_cast<int>(j);
            }
        }

        if (cmp != -1) {
            graph.setEdgeCmp(static_cast<int>(i), cmp);
            graph.setEdgeCmp(cmp, static_cast<int>(i));
        }
    }
}

} // namespace

Graph pg_to_sdsl_graph(const PgGraph &pg) {
    if (pg.directed_edges.size() != pg.m * 2) {
        throw std::runtime_error("pg graph has inconsistent directed edge count");
    }

    Graph graph(static_cast<unsigned int>(pg.n), static_cast<unsigned int>(pg.m));

    for (std::size_t i = 0; i < pg.directed_edges.size(); ++i) {
        const auto [source, target] = pg.directed_edges[i];
        graph.setEdge(static_cast<int>(i), source, target);

        if (i == 0) {
            graph.setVertexFirst(source, 0);
        } else if (source != graph.getEdgeSrc(static_cast<int>(i - 1))) {
            graph.setVertexLast(graph.getEdgeSrc(static_cast<int>(i - 1)), static_cast<unsigned int>(i - 1));
            graph.setVertexFirst(source, static_cast<unsigned int>(i));
        }
    }

    if (!pg.directed_edges.empty()) {
        graph.setVertexLast(graph.getEdgeSrc(static_cast<int>(pg.directed_edges.size() - 1)),
                            static_cast<unsigned int>(pg.directed_edges.size() - 1));
    }

    link_reverse_edges(graph, pg.directed_edges.size());
    return graph;
}

std::vector<ComponentPgGraph> extract_pg_components(const PgGraph &pg) {
    const AdjacencyList full = pg_to_adjacency_list(pg);
    const std::vector<std::uint32_t> labels = component_labels(full);
    const std::size_t components = component_count(labels);

    std::vector<ComponentPgGraph> result(components);
    std::vector<std::vector<std::uint32_t>> global_vertices(components);

    for (std::uint32_t v = 0; v < labels.size(); ++v) {
        global_vertices[labels[v]].push_back(v);
    }

    for (std::size_t cid = 0; cid < components; ++cid) {
        auto &component = result[cid];
        component.local_to_global = std::move(global_vertices[cid]);

        std::vector<std::int32_t> global_to_local(pg.n, -1);
        for (std::uint32_t local = 0; local < component.local_to_global.size(); ++local) {
            global_to_local[component.local_to_global[local]] = static_cast<std::int32_t>(local);
        }

        std::vector<std::pair<std::uint32_t, std::uint32_t>> filtered_edges;
        filtered_edges.reserve(component.local_to_global.size() * 4);

        for (const auto &[u, v] : pg.directed_edges) {
            if (labels[u] != static_cast<std::uint32_t>(cid) || labels[v] != static_cast<std::uint32_t>(cid)) {
                continue;
            }
            filtered_edges.emplace_back(static_cast<std::uint32_t>(global_to_local[u]),
                                        static_cast<std::uint32_t>(global_to_local[v]));
        }

        std::unordered_set<std::uint64_t> unique_undirected;
        unique_undirected.reserve(filtered_edges.size());
        for (const auto &[u, v] : filtered_edges) {
            const std::uint32_t a = u < v ? u : v;
            const std::uint32_t b = u < v ? v : u;
            unique_undirected.insert((static_cast<std::uint64_t>(a) << 32) | b);
        }

        component.pg.n = component.local_to_global.size();
        component.pg.m = unique_undirected.size();
        component.pg.directed_edges = std::move(filtered_edges);
    }

    return result;
}

Graph read_sdsl_graph(const std::string &path) {
    return pg_to_sdsl_graph(read_pg_graph(path));
}

} // namespace mp2
