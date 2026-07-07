#include "components.hpp"

#include <algorithm>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace mp2 {

std::vector<std::uint32_t> component_labels(const AdjacencyList &graph) {
    const std::size_t n = graph.vertices();
    std::vector<std::uint32_t> labels(n, static_cast<std::uint32_t>(-1));
    std::uint32_t current = 0;

    for (std::uint32_t start = 0; start < n; ++start) {
        if (labels[start] != static_cast<std::uint32_t>(-1)) {
            continue;
        }

        std::queue<std::uint32_t> queue;
        labels[start] = current;
        queue.push(start);

        while (!queue.empty()) {
            const std::uint32_t u = queue.front();
            queue.pop();
            for (const std::uint32_t v : graph.adjacent(u)) {
                if (labels[v] == static_cast<std::uint32_t>(-1)) {
                    labels[v] = current;
                    queue.push(v);
                }
            }
        }

        ++current;
    }

    return labels;
}

std::size_t component_count(const std::vector<std::uint32_t> &labels) {
    if (labels.empty()) {
        return 0;
    }
    return static_cast<std::size_t>(*std::max_element(labels.begin(), labels.end()) + 1);
}

std::vector<ComponentGraph> extract_components(const AdjacencyList &graph) {
    const std::vector<std::uint32_t> labels = component_labels(graph);
    const std::size_t k = component_count(labels);

    std::vector<ComponentGraph> components(k);
    std::vector<std::unordered_map<std::uint32_t, std::uint32_t>> global_to_local(k);

    for (std::uint32_t v = 0; v < labels.size(); ++v) {
        const std::uint32_t cid = labels[v];
        auto &mapping = global_to_local[cid];
        const std::uint32_t local = static_cast<std::uint32_t>(mapping.size());
        mapping.emplace(v, local);
        components[cid].local_to_global.push_back(v);
    }

    for (std::size_t cid = 0; cid < k; ++cid) {
        components[cid].graph = AdjacencyList(components[cid].local_to_global.size());
    }

    for (std::uint32_t u = 0; u < labels.size(); ++u) {
        const std::uint32_t cid = labels[u];
        const auto &map = global_to_local[cid];
        for (const std::uint32_t v : graph.adjacent(u)) {
            if (u < v && labels[v] == cid) {
                components[cid].graph.add_edge(map.at(u), map.at(v));
            }
        }
    }

    for (auto &component : components) {
        if (component.local_to_global.size() != component.graph.vertices()) {
            throw std::logic_error("component vertex count mismatch after extraction");
        }
    }

    std::sort(components.begin(), components.end(),
              [](const ComponentGraph &a, const ComponentGraph &b) {
                  return a.local_to_global.size() > b.local_to_global.size();
              });

    return components;
}

} // namespace mp2
