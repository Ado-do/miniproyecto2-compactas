#include "adjacency_list.hpp"
#include "bench_common.hpp"
#include "graph_io.hpp"
#include "k2_tree.hpp"
#include "pemb_graph.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

int main(int argc, char **argv) {
    mp2::bench_cli::RunConfig cfg;
    if (!mp2::bench_cli::parse_run_config(argc, argv, cfg, argv[0])) {
        return 1;
    }

    if (!std::filesystem::exists(cfg.graph_path)) {
        std::cerr << "Graph not found: " << cfg.graph_path << '\n';
        return 1;
    }

    try {
        const std::string graph_name = std::filesystem::path(cfg.graph_path).filename().string();

        auto t0 = std::chrono::high_resolution_clock::now();
        const mp2::PgGraph pg = mp2::read_pg_graph(cfg.graph_path);
        const mp2::AdjacencyList adjacency = mp2::pg_to_adjacency_list(pg);
        auto t1 = std::chrono::high_resolution_clock::now();
        const double adjacency_build_ms = mp2::bench_cli::elapsed_ms(t0, t1);

        const std::size_t n = adjacency.vertices();
        const std::size_t m = adjacency.edges();
        const auto queries = mp2::bench::make_queries(n, cfg.ops, cfg.seed);

        mp2::K2BuildOptions k2_opts;
        if (cfg.k2_max_level.has_value()) {
            k2_opts.max_level = cfg.k2_max_level;
        }

        std::unique_ptr<mp2::K2TreeGraph> k2;
        double k2_build_ms = 0;
        if (!cfg.skip_k2) {
            t0 = std::chrono::high_resolution_clock::now();
            k2 = std::make_unique<mp2::K2TreeGraph>(
                mp2::K2TreeGraph::build(adjacency, cfg.k2_basename, k2_opts));
            t1 = std::chrono::high_resolution_clock::now();
            k2_build_ms = mp2::bench_cli::elapsed_ms(t0, t1);
        }

        std::unique_ptr<mp2::PembGraph> pemb;
        double pemb_build_ms = 0;
        if (!cfg.skip_pemb) {
            t0 = std::chrono::high_resolution_clock::now();
            pemb = std::make_unique<mp2::PembGraph>(mp2::PembGraph::from_pg_graph(pg));
            t1 = std::chrono::high_resolution_clock::now();
            pemb_build_ms = mp2::bench_cli::elapsed_ms(t0, t1);
        }

        const auto adj_degree = mp2::bench_cli::bench_degree(adjacency, queries, cfg.ops, cfg.reps);
        const auto adj_neighbors =
            mp2::bench_cli::bench_neighbors(adjacency, queries, cfg.ops, cfg.reps);

        std::cout << "mp2_bench run\n";
        std::cout << "graph: " << graph_name << "  n=" << n << "  m=" << m << '\n';
        std::cout << "ops=" << cfg.ops << " reps=" << cfg.reps << " seed=" << cfg.seed << "\n\n";

        std::cout << "Build times (ms):\n";
        std::cout << "  adjacency: " << adjacency_build_ms << '\n';
        if (k2) {
            std::cout << "  k2-tree:   " << k2_build_ms << " (max_level=" << k2->max_level() << ")\n";
        }
        if (pemb) {
            std::cout << "  pemb:      " << pemb_build_ms << '\n';
        }
        std::cout << '\n';

        std::cout << "Space (bytes):\n";
        std::cout << "  adjacency: " << adjacency.size_bytes() << '\n';
        if (k2) {
            std::cout << "  k2-tree:   " << k2->size_bytes_on_disk() << " (on disk)\n";
        }
        if (pemb) {
            std::cout << "  pemb:      " << pemb->size_bytes() << " (compact)\n";
        }
        std::cout << '\n';

        std::cout << std::left << std::setw(14) << "representation" << std::setw(12) << "operation"
                  << "avg time\n";
        mp2::bench_cli::print_timing_row("adjacency", "degree", adj_degree);
        mp2::bench_cli::print_timing_row("adjacency", "neighbors", adj_neighbors);

        std::unique_ptr<mp2::bench::TimingStats> k2_degree;
        std::unique_ptr<mp2::bench::TimingStats> k2_neighbors;
        if (k2) {
            k2_degree = std::make_unique<mp2::bench::TimingStats>(
                mp2::bench_cli::bench_degree(*k2, queries, cfg.ops, cfg.reps));
            k2_neighbors = std::make_unique<mp2::bench::TimingStats>(
                mp2::bench_cli::bench_neighbors(*k2, queries, cfg.ops, cfg.reps));
            mp2::bench_cli::print_timing_row("k2-tree", "degree", *k2_degree);
            mp2::bench_cli::print_timing_row("k2-tree", "neighbors", *k2_neighbors);
        }

        std::unique_ptr<mp2::bench::TimingStats> pemb_degree;
        std::unique_ptr<mp2::bench::TimingStats> pemb_neighbors;
        if (pemb) {
            pemb_degree = std::make_unique<mp2::bench::TimingStats>(
                mp2::bench_cli::bench_degree(*pemb, queries, cfg.ops, cfg.reps));
            pemb_neighbors = std::make_unique<mp2::bench::TimingStats>(
                mp2::bench_cli::bench_neighbors(*pemb, queries, cfg.ops, cfg.reps));
            mp2::bench_cli::print_timing_row("pemb", "degree", *pemb_degree);
            mp2::bench_cli::print_timing_row("pemb", "neighbors", *pemb_neighbors);
        }

        if (!cfg.out_csv.empty()) {
            std::filesystem::create_directories(std::filesystem::path(cfg.out_csv).parent_path());
            std::ofstream out(cfg.out_csv, std::ios::app);
            mp2::bench_cli::ensure_csv_header(out, cfg.out_csv, k2 != nullptr);
            mp2::bench_cli::append_csv_row(out, cfg, graph_name, "adjacency", n, m,
                                           adjacency.size_bytes(), "degree", adj_degree);
            mp2::bench_cli::append_csv_row(out, cfg, graph_name, "adjacency", n, m,
                                           adjacency.size_bytes(), "neighbors", adj_neighbors);
            if (k2 && k2_degree && k2_neighbors) {
                mp2::bench_cli::append_csv_row(out, cfg, graph_name, "k2-tree", n, m,
                                               k2->size_bytes_on_disk(), "degree", *k2_degree,
                                               k2->max_level());
                mp2::bench_cli::append_csv_row(out, cfg, graph_name, "k2-tree", n, m,
                                               k2->size_bytes_on_disk(), "neighbors", *k2_neighbors,
                                               k2->max_level());
            }
            if (pemb && pemb_degree && pemb_neighbors) {
                mp2::bench_cli::append_csv_row(out, cfg, graph_name, "pemb", n, m, pemb->size_bytes(),
                                               "degree", *pemb_degree);
                mp2::bench_cli::append_csv_row(out, cfg, graph_name, "pemb", n, m, pemb->size_bytes(),
                                               "neighbors", *pemb_neighbors);
            }
            std::cout << "\nCSV appended: " << cfg.out_csv << '\n';
        }

        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "mp2_bench failed: " << ex.what() << '\n';
        return 1;
    }
}
