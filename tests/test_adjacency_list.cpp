#include "adjacency_list.hpp"
#include "graph_io.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>

namespace {

void expect_eq(std::size_t got, std::size_t expected, const char *label) {
    if (got != expected) {
        std::cerr << label << ": expected " << expected << ", got " << got << '\n';
        std::exit(1);
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

    expect_eq(graph.vertices(), 5, "enunciado vertices");
    expect_eq(graph.edges(), 7, "enunciado edges");
    expect_eq(graph.degree(0), 2, "enunciado degree(0)");
    expect_eq(graph.degree(4), 2, "enunciado degree(4)");
    if (!graph.neighbors(1, 3) || graph.neighbors(0, 4)) {
        std::cerr << "enunciado neighbors mismatch\n";
        std::exit(1);
    }
    if (graph.size_bytes() == 0) {
        std::cerr << "enunciado size_bytes is zero\n";
        std::exit(1);
    }
}

void compare_with_pg(const std::string &path, std::size_t sample_pairs) {
    if (!std::filesystem::exists(path)) {
        return;
    }

    const mp2::PgGraph pg = mp2::read_pg_graph(path);
    const mp2::AdjacencyList graph = mp2::pg_to_adjacency_list(pg);

    std::mt19937 rng(42);
    std::uniform_int_distribution<std::uint32_t> vertex_dist(0, static_cast<std::uint32_t>(graph.vertices() - 1));

    for (std::size_t i = 0; i < sample_pairs; ++i) {
        const std::uint32_t v = vertex_dist(rng);
        const std::size_t brute = graph.adjacent(v).size();
        if (graph.degree(v) != brute) {
            std::cerr << path << ": degree mismatch at vertex " << v << '\n';
            std::exit(1);
        }
    }

    for (std::size_t i = 0; i < sample_pairs; ++i) {
        const std::uint32_t u = vertex_dist(rng);
        const std::uint32_t v = vertex_dist(rng);
        const bool from_list =
            std::find(graph.adjacent(u).begin(), graph.adjacent(u).end(), v) != graph.adjacent(u).end();
        if (graph.neighbors(u, v) != from_list) {
            std::cerr << path << ": neighbors mismatch at (" << u << "," << v << ")\n";
            std::exit(1);
        }
    }
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
        compare_with_pg(graphs + "/tiger_map_hawaii.pg", 5000);
        compare_with_pg(graphs + "/planar_embedding1000000.pg", 2000);

        std::cout << "test_adjacency_list ok\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "test_adjacency_list failed: " << ex.what() << '\n';
        return 1;
    }
}
