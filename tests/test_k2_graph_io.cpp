#include "components.hpp"
#include "graph_io.hpp"
#include "k2_graph_io.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace {

bool graphs_equal_sorted(const mp2::AdjacencyList &lhs, const mp2::AdjacencyList &rhs) {
    if (lhs.vertices() != rhs.vertices() || lhs.edges() != rhs.edges()) {
        return false;
    }

    for (std::uint32_t v = 0; v < lhs.vertices(); ++v) {
        if (lhs.degree(v) != rhs.degree(v)) {
            return false;
        }
        for (std::uint32_t u = 0; u < lhs.vertices(); ++u) {
            if (lhs.neighbors(v, u) != rhs.neighbors(v, u)) {
                return false;
            }
        }
    }
    return true;
}

void test_enunciado_example() {
  // 5 vertices, 7 undirected edges from the course statement.
    mp2::AdjacencyList graph(5);
    graph.add_edge(0, 1);
    graph.add_edge(0, 2);
    graph.add_edge(1, 2);
    graph.add_edge(1, 3);
    graph.add_edge(2, 3);
    graph.add_edge(2, 4);
    graph.add_edge(3, 4);

    const std::string path = "/tmp/mp2_k2_enunciado.k2g";
    mp2::write_k2_binary(graph, path);

    const mp2::AdjacencyList rebuilt = mp2::read_k2_binary(path);
    if (!graphs_equal_sorted(graph, rebuilt)) {
        std::cerr << "enunciado example: round-trip mismatch\n";
        std::exit(1);
    }
}

void test_two_component_pipeline() {
    mp2::AdjacencyList graph(6);
    graph.add_edge(0, 1);
    graph.add_edge(1, 2);
    graph.add_edge(2, 0);
    graph.add_edge(3, 4);
    graph.add_edge(4, 5);
    graph.add_edge(5, 3);

    const std::string pg_path = "/tmp/mp2_two_components.pg";
    {
        std::ofstream out(pg_path);
        out << "6 6\n";
        out << "0 1 1 0 0 2 2 0 1 2 2 1 3 4 4 3 3 5 5 3 4 5 5 4\n";
    }

    const std::string out_dir = "/tmp/mp2_k2_components";
    std::filesystem::remove_all(out_dir);

    const auto files = mp2::write_pg_components_k2_binaries(pg_path, out_dir, "two");
    if (files.size() != 2) {
        std::cerr << "two-component pipeline: expected 2 files, got " << files.size() << '\n';
        std::exit(1);
    }

    const auto comp0 = mp2::read_k2_binary(files[0]);
    const auto comp1 = mp2::read_k2_binary(files[1]);
    if (comp0.vertices() != 3 || comp1.vertices() != 3) {
        std::cerr << "two-component pipeline: unexpected component sizes\n";
        std::exit(1);
    }
    if (comp0.edges() != 3 || comp1.edges() != 3) {
        std::cerr << "two-component pipeline: unexpected edge counts\n";
        std::exit(1);
    }
}

void test_dataset_if_present(const std::string &path) {
    if (!std::filesystem::exists(path)) {
        return;
    }

    const mp2::AdjacencyList original = mp2::pg_to_adjacency_list(mp2::read_pg_graph(path));
    const std::string k2_path = "/tmp/mp2_k2_dataset.k2g";
    mp2::write_k2_binary(original, k2_path);
    const mp2::AdjacencyList rebuilt = mp2::read_k2_binary(k2_path);

    if (!graphs_equal_sorted(original, rebuilt)) {
        std::cerr << path << ": k2 round-trip mismatch\n";
        std::exit(1);
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
        test_two_component_pipeline();
        test_dataset_if_present(graphs + "/tiger_map_hawaii.pg");

        std::cout << "test_k2_graph_io ok\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "test_k2_graph_io failed: " << ex.what() << '\n';
        return 1;
    }
}
