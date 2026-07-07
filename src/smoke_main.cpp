#include "graph_io.hpp"

#include <iostream>
#include <string>

int main(int argc, char **argv) {
    if (argc == 2 && std::string(argv[1]) == "--help") {
        std::cout << "Usage: mp2_smoke <path-to-graph.pg>\n";
        std::cout << "Build smoke test for miniproyecto 2 (adjacency list + linked libs).\n";
        return 0;
    }

    if (argc != 2) {
        std::cerr << "Usage: mp2_smoke <path-to-graph.pg>\n";
        return 1;
    }

    try {
        const mp2::PgGraph pg = mp2::read_pg_graph(argv[1]);
        const mp2::AdjacencyList graph = mp2::pg_to_adjacency_list(pg);

        std::cout << "mp2_smoke ok\n";
        std::cout << "vertices: " << graph.vertices() << '\n';
        std::cout << "edges: " << graph.edges() << '\n';
        std::cout << "adjacency_bytes: " << graph.size_bytes() << '\n';
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "mp2_smoke failed: " << ex.what() << '\n';
        return 1;
    }
}
