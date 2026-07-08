#include "adjacency_list.hpp"
#include "graph_io.hpp"
#include "pemb_graph.hpp"
#include "planar_graph.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

namespace {

void compare_with_baseline(const mp2::AdjacencyList &baseline, const mp2::PembGraph &pemb,
                           std::size_t sample_vertices, std::size_t sample_pairs) {
    if (baseline.vertices() != pemb.vertices()) {
        std::cerr << "vertex count mismatch\n";
        std::exit(1);
    }

    std::mt19937 rng(11);
    std::uniform_int_distribution<std::uint32_t> vertex_dist(
        0, static_cast<std::uint32_t>(baseline.vertices() - 1));

    for (std::size_t i = 0; i < sample_vertices; ++i) {
        const std::uint32_t v = vertex_dist(rng);
        if (baseline.degree(v) != pemb.degree(v)) {
            std::cerr << "degree mismatch at v=" << v << '\n';
            std::exit(1);
        }
    }

    for (std::size_t i = 0; i < sample_pairs; ++i) {
        const std::uint32_t u = vertex_dist(rng);
        const std::uint32_t v = vertex_dist(rng);
        if (baseline.neighbors(u, v) != pemb.neighbors(u, v)) {
            std::cerr << "neighbors mismatch at (" << u << "," << v << ")\n";
            std::exit(1);
        }
    }
}

void test_enunciado_example() {
    const std::string path = "/tmp/mp2_enunciado.pg";
    {
        std::ofstream out(path);
        out << "5\n7\n";
        out << "0 1\n0 2\n0 3\n0 4\n";
        out << "1 0\n1 4\n1 2\n";
        out << "2 0\n2 1\n";
        out << "3 0\n3 4\n";
        out << "4 0\n4 3\n4 1\n";
    }

    const mp2::PgGraph pg = mp2::read_pg_graph(path);
    const mp2::AdjacencyList baseline = mp2::pg_to_adjacency_list(pg);
    const mp2::PembGraph pemb = mp2::PembGraph::from_pg_graph(pg);

    compare_with_baseline(baseline, pemb, 5, 25);
    if (pemb.size_bytes() == 0) {
        std::cerr << "pemb size_bytes is zero\n";
        std::exit(1);
    }
}

void test_two_component_extraction() {
    const std::string path = "/tmp/mp2_pemb_two_components.pg";
    {
        std::ofstream out(path);
        out << "6\n6\n";
        out << "0 1\n1 0\n0 2\n2 0\n1 2\n2 1\n";
        out << "3 4\n4 3\n3 5\n5 3\n4 5\n5 4\n";
    }

    const auto components = mp2::extract_pg_components(mp2::read_pg_graph(path));
    if (components.size() != 2) {
        std::cerr << "expected 2 pg components, got " << components.size() << '\n';
        std::exit(1);
    }
    if (components[0].pg.n != 3 || components[1].pg.n != 3) {
        std::cerr << "unexpected pg component sizes\n";
        std::exit(1);
    }
}

void test_triangle_pemb() {
    const std::string path = "/tmp/mp2_triangle.pg";
    {
        std::ofstream out(path);
        out << "3\n3\n";
        out << "0 1\n0 2\n1 0\n1 2\n2 0\n2 1\n";
    }

    const mp2::PembGraph pemb = mp2::PembGraph::from_pg_file(path);
    if (pemb.vertices() != 3 || pemb.degree(0) != 2) {
        std::cerr << "triangle pemb mismatch\n";
        std::exit(1);
    }
}

void test_dataset_if_present(const std::string &path) {
    if (!std::filesystem::exists(path)) {
        return;
    }

    const mp2::PgGraph pg = mp2::read_pg_graph(path);
    const mp2::AdjacencyList baseline = mp2::pg_to_adjacency_list(pg);
    const mp2::PembGraph pemb = mp2::PembGraph::from_pg_graph(pg);
    compare_with_baseline(baseline, pemb, 2000, 5000);

    if (pemb.size_bytes() == 0) {
        std::cerr << path << ": pemb size_bytes is zero\n";
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
        test_two_component_extraction();
        test_triangle_pemb();
        test_dataset_if_present(graphs + "/tiger_map_hawaii.pg");
        test_dataset_if_present(graphs + "/planar_embedding1000000.pg");

        std::cout << "test_pemb_graph ok\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "test_pemb_graph failed: " << ex.what() << '\n';
        return 1;
    }
}
