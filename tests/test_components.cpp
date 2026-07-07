#include "components.hpp"
#include "graph_io.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void expect_components(const std::string &path, std::size_t expected) {
    const auto components = mp2::read_pg_components(path);
    if (components.size() != expected) {
        std::cerr << path << ": expected " << expected << " components, got " << components.size()
                  << '\n';
        std::exit(1);
    }

    std::size_t total_vertices = 0;
    std::size_t total_edges = 0;
    for (const auto &component : components) {
        total_vertices += component.graph.vertices();
        total_edges += component.graph.edges();
    }

    const mp2::PgGraph pg = mp2::read_pg_graph(path);
    const mp2::AdjacencyList full = mp2::pg_to_adjacency_list(pg);
    if (total_vertices != full.vertices() || total_edges != full.edges()) {
        std::cerr << path << ": partition does not preserve n/m\n";
        std::exit(1);
    }
}

void test_synthetic_two_components() {
    mp2::AdjacencyList graph(6);
    graph.add_edge(0, 1);
    graph.add_edge(1, 2);
    graph.add_edge(2, 0);
    graph.add_edge(3, 4);
    graph.add_edge(4, 5);
    graph.add_edge(5, 3);

    const auto components = mp2::extract_components(graph);
    if (components.size() != 2) {
        std::cerr << "synthetic: expected 2 components, got " << components.size() << '\n';
        std::exit(1);
    }
    if (components[0].graph.vertices() != 3 || components[1].graph.vertices() != 3) {
        std::cerr << "synthetic: unexpected component sizes\n";
        std::exit(1);
    }
}

} // namespace

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <dataset-root>\n";
        std::cerr << "  dataset-root should contain material-base/datasets/graphs\n";
        return 1;
    }

    const std::string root = argv[1];
    const std::string graphs = root + "/material-base/datasets/graphs";

    try {
        test_synthetic_two_components();

        // Hawaii .pg is one connected graph (33k nodes); "17 components" on the UdeC site
        // refers to geographic TIGER regions, not graph connectivity in the .pg file.
        expect_components(graphs + "/tiger_map_hawaii.pg", 1);
        expect_components(graphs + "/planar_embedding1000000.pg", 1);
        expect_components(graphs + "/worldcitiespop.pg", 1);

        std::cout << "test_components ok\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "test_components failed: " << ex.what() << '\n';
        return 1;
    }
}
