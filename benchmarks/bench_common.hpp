#pragma once

#include "benchmark.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>

namespace mp2::bench_cli {

struct RunConfig {
    std::string graph_path;
    std::string out_csv;
    std::string k2_basename = "/tmp/mp2_bench_k2";
    std::size_t ops = 100;
    std::size_t reps = 3;
    std::uint32_t seed = 42;
    bool skip_k2 = false;
    bool skip_pemb = false;
    std::optional<std::uint32_t> k2_max_level;
};

inline double elapsed_ms(const std::chrono::high_resolution_clock::time_point &t0,
                         const std::chrono::high_resolution_clock::time_point &t1) {
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

inline void append_csv_row(std::ofstream &out, const RunConfig &cfg, const std::string &graph_name,
                           const std::string &representation, std::size_t n, std::size_t m,
                           std::size_t space_bytes, const std::string &operation,
                           const mp2::bench::TimingStats &stats,
                           const std::optional<std::uint32_t> &k2_max_level = std::nullopt) {
    out << graph_name << ',' << representation << ",0," << n << ',' << m << ',' << space_bytes << ','
        << operation << ',' << std::fixed << std::setprecision(3) << stats.per_op_mean_us << ','
        << stats.per_op_std_us << ',' << cfg.ops << ',' << cfg.reps << ',' << cfg.seed;
    if (k2_max_level.has_value()) {
        out << ',' << *k2_max_level;
    }
    out << '\n';
}

inline void ensure_csv_header(std::ofstream &out, const std::string &path, bool with_k2_level) {
    const bool write_header = !std::filesystem::exists(path);
    if (!write_header) {
        return;
    }
    out << "graph,representation,component_id,n,m,space_bytes,operation,avg_time_us,std_time_us,ops,"
           "reps,seed";
    if (with_k2_level) {
        out << ",k2_max_level";
    }
    out << '\n';
}

inline void print_timing_row(const std::string &representation, const std::string &operation,
                             const mp2::bench::TimingStats &stats) {
    std::cout << std::left << std::setw(14) << representation << std::setw(12) << operation
              << std::fixed << std::setprecision(3) << stats.per_op_mean_us << " us/op"
              << " (batch " << stats.batch_mean_us << " +/- " << stats.batch_std_us << " us)\n";
}

template <typename Graph>
inline mp2::bench::TimingStats bench_degree(const Graph &graph, const mp2::bench::QueryBatch &queries,
                                           std::size_t ops, std::size_t reps) {
    return mp2::bench::time_batch(ops, reps, [&] {
        for (const std::uint32_t v : queries.degree_vertices) {
            (void)graph.degree(v);
        }
    });
}

template <typename Graph>
inline mp2::bench::TimingStats bench_neighbors(const Graph &graph, const mp2::bench::QueryBatch &queries,
                                              std::size_t ops, std::size_t reps) {
    return mp2::bench::time_batch(ops, reps, [&] {
        for (const auto &[u, v] : queries.neighbor_pairs) {
            (void)graph.neighbors(u, v);
        }
    });
}

inline bool parse_run_config(int argc, char **argv, RunConfig &cfg, const char *prog) {
    if (argc < 2) {
        std::cerr << "Usage: " << prog
                  << " <graph.pg> [--ops N] [--reps N] [--seed N] [--out path.csv]\n"
                  << "       [--k2-basename path] [--k2-max-level N] [--skip-k2] [--skip-pemb]\n";
        return false;
    }

    cfg.graph_path = argv[1];
    for (int i = 2; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--ops" && i + 1 < argc) {
            cfg.ops = static_cast<std::size_t>(std::stoul(argv[++i]));
        } else if (arg == "--reps" && i + 1 < argc) {
            cfg.reps = static_cast<std::size_t>(std::stoul(argv[++i]));
        } else if (arg == "--seed" && i + 1 < argc) {
            cfg.seed = static_cast<std::uint32_t>(std::stoul(argv[++i]));
        } else if (arg == "--out" && i + 1 < argc) {
            cfg.out_csv = argv[++i];
        } else if (arg == "--k2-basename" && i + 1 < argc) {
            cfg.k2_basename = argv[++i];
        } else if (arg == "--k2-max-level" && i + 1 < argc) {
            cfg.k2_max_level = static_cast<std::uint32_t>(std::stoul(argv[++i]));
        } else if (arg == "--skip-k2") {
            cfg.skip_k2 = true;
        } else if (arg == "--skip-pemb") {
            cfg.skip_pemb = true;
        } else {
            std::cerr << "Unknown argument: " << arg << '\n';
            return false;
        }
    }
    return true;
}

} // namespace mp2::bench_cli
