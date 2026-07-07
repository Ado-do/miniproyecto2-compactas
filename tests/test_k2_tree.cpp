#include "adjacency_list.hpp"
#include "graph_io.hpp"
#include "k2_tree.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>

namespace {

void compare_graphs(const mp2::AdjacencyList &baseline, const mp2::K2TreeGraph &k2,
                    std::size_t sample_vertices, std::size_t sample_pairs) {
    if (baseline.vertices() != k2.vertices()) {
        std::cerr << "vertex count mismatch\n";
        std::exit(1);
    }

    std::mt19937 rng(7);
    std::uniform_int_distribution<std::uint32_t> vertex_dist(
        0, static_cast<std::uint32_t>(baseline.vertices() - 1));

    for (std::size_t i = 0; i < sample_vertices; ++i) {
        const std::uint32_t v = vertex_dist(rng);
        if (baseline.degree(v) != k2.degree(v)) {
            std::cerr << "degree mismatch at v=" << v << ": baseline=" << baseline.degree(v)
                      << " k2=" << k2.degree(v) << '\n';
            std::exit(1);
        }
    }

    for (std::size_t i = 0; i < sample_pairs; ++i) {
        const std::uint32_t u = vertex_dist(rng);
        const std::uint32_t v = vertex_dist(rng);
        if (baseline.neighbors(u, v) != k2.neighbors(u, v)) {
            std::cerr << "neighbors mismatch at (" << u << "," << v << ")\n";
            std::exit(1);
        }
    }
}

void test_enunciado_example() {
    mp2::AdjacencyList graph(5);
    graph.add_edge(0, 1);
    graph.add_edge(0, 2);
    graph.add_edge(1, 2);
    graph.add_edge(1, 3);
    graph.add_edge(2, 3);
    graph.add_edge(2, 4);
    graph.add_edge(3, 4);

    const std::string basename = "/tmp/mp2_k2_wrapper_enunciado";
    const auto k2 = mp2::K2TreeGraph::build(graph, basename);
    compare_graphs(graph, k2, 5, 25);

    if (k2.size_bytes_on_disk() == 0) {
        std::cerr << "k2-tree file size is zero\n";
        std::exit(1);
    }
}

void test_dataset_if_present(const std::string &path, const std::string &basename) {
    if (!std::filesystem::exists(path)) {
        return;
    }

    const mp2::AdjacencyList graph = mp2::pg_to_adjacency_list(mp2::read_pg_graph(path));
    const auto k2 = mp2::K2TreeGraph::build(graph, basename);
    compare_graphs(graph, k2, 2000, 5000);

    const auto reloaded = mp2::K2TreeGraph::load(basename);
    compare_graphs(graph, reloaded, 1000, 2000);
}

} // namespace

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <dataset-root>\n";
        return 1;
    }

    const std::string root = argv[1];
    const std::string graphs = root + "/material-base/datasets/graphs";

    try {
        test_enunciado_example();
        test_dataset_if_present(graphs + "/tiger_map_hawaii.pg", "/tmp/mp2_k2_wrapper_hawaii");

        std::cout << "test_k2_tree ok\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "test_k2_tree failed: " << ex.what() << '\n';
        return 1;
    }
}
