#include "adjacency_list.hpp"
#include "bench_common.hpp"
#include "graph_io.hpp"
#include "k2_tree.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace {

struct TuneConfig {
    std::string graph_path;
    std::string out_csv = "results/k2_tune.csv";
    std::string best_csv = "results/k2_best_params.csv";
    std::string k2_basename = "/tmp/mp2_bench_k2_tune";
    std::size_t ops = 200;
    std::size_t reps = 3;
    std::size_t verify_samples = 500;
    std::uint32_t seed = 42;
    int level_span = 2;
};

struct TuneRow {
    std::uint32_t max_level = 0;
    std::size_t space_bytes = 0;
    double build_ms = 0;
    double degree_us = 0;
    double neighbors_us = 0;
    bool correct = false;
};

void print_tune_usage(const char *prog) {
    std::cerr << "Usage: " << prog
              << " <graph.pg> [--out path.csv] [--best path.csv] [--ops N] [--reps N]\n"
              << "       [--seed N] [--verify N] [--level-span N] [--k2-basename path]\n"
              << "\nSweeps max_level around the auto value for k2-tree Basic (k=2 fixed).\n";
}

bool parse_tune_config(int argc, char **argv, TuneConfig &cfg) {
    if (argc < 2) {
        return false;
    }
    cfg.graph_path = argv[1];
    for (int i = 2; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--out" && i + 1 < argc) {
            cfg.out_csv = argv[++i];
        } else if (arg == "--best" && i + 1 < argc) {
            cfg.best_csv = argv[++i];
        } else if (arg == "--ops" && i + 1 < argc) {
            cfg.ops = static_cast<std::size_t>(std::stoul(argv[++i]));
        } else if (arg == "--reps" && i + 1 < argc) {
            cfg.reps = static_cast<std::size_t>(std::stoul(argv[++i]));
        } else if (arg == "--seed" && i + 1 < argc) {
            cfg.seed = static_cast<std::uint32_t>(std::stoul(argv[++i]));
        } else if (arg == "--verify" && i + 1 < argc) {
            cfg.verify_samples = static_cast<std::size_t>(std::stoul(argv[++i]));
        } else if (arg == "--level-span" && i + 1 < argc) {
            cfg.level_span = std::stoi(argv[++i]);
        } else if (arg == "--k2-basename" && i + 1 < argc) {
            cfg.k2_basename = argv[++i];
        } else {
            std::cerr << "Unknown argument: " << arg << '\n';
            return false;
        }
    }
    return true;
}

bool verify_k2(const mp2::AdjacencyList &baseline, const mp2::K2TreeGraph &k2,
               std::size_t samples, std::uint32_t seed) {
    if (baseline.vertices() != k2.vertices()) {
        return false;
    }
    if (baseline.vertices() == 0) {
        return true;
    }

    std::mt19937 rng(seed);
    std::uniform_int_distribution<std::uint32_t> vertex_dist(
        0, static_cast<std::uint32_t>(baseline.vertices() - 1));

    for (std::size_t i = 0; i < samples; ++i) {
        const std::uint32_t v = vertex_dist(rng);
        if (baseline.degree(v) != k2.degree(v)) {
            return false;
        }
        const std::uint32_t u = vertex_dist(rng);
        const std::uint32_t w = vertex_dist(rng);
        if (baseline.neighbors(u, w) != k2.neighbors(u, w)) {
            return false;
        }
    }
    return true;
}

/// k2-tree Basic calls exit(-1) on invalid max_level; isolate construction in a child process.
bool build_k2_isolated(const mp2::AdjacencyList &adjacency, const std::string &basename,
                       std::uint32_t max_level, double &build_ms) {
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        return false;
    }

    const pid_t pid = fork();
    if (pid < 0) {
        close(pipefd[0]);
        close(pipefd[1]);
        return false;
    }

    if (pid == 0) {
        close(pipefd[0]);
        const int devnull = open("/dev/null", O_WRONLY);
        if (devnull >= 0) {
            dup2(devnull, STDERR_FILENO);
            close(devnull);
        }
        const auto t0 = std::chrono::high_resolution_clock::now();
        try {
            mp2::K2BuildOptions opts;
            opts.max_level = max_level;
            (void)mp2::K2TreeGraph::build(adjacency, basename, opts);
            const auto t1 = std::chrono::high_resolution_clock::now();
            build_ms = mp2::bench_cli::elapsed_ms(t0, t1);
            (void)write(pipefd[1], &build_ms, sizeof(build_ms));
            close(pipefd[1]);
            _exit(0);
        } catch (...) {
            close(pipefd[1]);
            _exit(1);
        }
    }

    close(pipefd[1]);
    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        close(pipefd[0]);
        return false;
    }

    bool ok = WIFEXITED(status) && WEXITSTATUS(status) == 0;
    if (ok) {
        const ssize_t nbytes = read(pipefd[0], &build_ms, sizeof(build_ms));
        ok = nbytes == static_cast<ssize_t>(sizeof(build_ms));
    }
    close(pipefd[0]);
    return ok;
}

void write_tune_header(std::ofstream &out, const std::string &path) {
    if (std::filesystem::exists(path)) {
        return;
    }
    out << "graph,n,m,auto_max_level,max_level,space_bytes,build_ms,degree_us,neighbors_us,"
           "correct,ops,reps,seed\n";
}

void write_best_header(std::ofstream &out, const std::string &path) {
    if (std::filesystem::exists(path)) {
        return;
    }
    out << "graph,n,m,auto_max_level,best_max_level,space_bytes,degree_us,neighbors_us,build_ms\n";
}

} // namespace

int main(int argc, char **argv) {
    TuneConfig cfg;
    if (!parse_tune_config(argc, argv, cfg)) {
        print_tune_usage(argv[0]);
        return 1;
    }

    if (!std::filesystem::exists(cfg.graph_path)) {
        std::cerr << "Graph not found: " << cfg.graph_path << '\n';
        return 1;
    }

    try {
        const std::string graph_name = std::filesystem::path(cfg.graph_path).filename().string();
        const mp2::AdjacencyList adjacency = mp2::pg_to_adjacency_list(mp2::read_pg_graph(cfg.graph_path));
        const std::size_t n = adjacency.vertices();
        const std::size_t m = adjacency.edges();
        const std::uint32_t auto_level = mp2::K2TreeGraph::auto_max_level(n);

        const int lo = static_cast<int>(auto_level);
        const int hi = static_cast<int>(auto_level) + cfg.level_span;

        std::cout << "mp2_bench_k2_tune\n";
        std::cout << "graph: " << graph_name << "  n=" << n << "  m=" << m << '\n';
        std::cout << "k2 Basic: k=2 fixed | auto_max_level=" << auto_level << " | sweep [" << lo
                  << ", " << hi << "] (levels < auto fallan en k2-tree Basic)\n\n";

        std::vector<TuneRow> rows;
        rows.reserve(static_cast<std::size_t>(hi - lo + 1));

        for (int level = lo; level <= hi; ++level) {
            const std::string basename = cfg.k2_basename + "_L" + std::to_string(level);
            TuneRow row;
            row.max_level = static_cast<std::uint32_t>(level);

            try {
                if (!build_k2_isolated(adjacency, basename, row.max_level, row.build_ms)) {
                    row.correct = false;
                    rows.push_back(row);
                    std::cout << "  L=" << level << " build failed (max_level invalido para este grafo)\n";
                    continue;
                }

                auto k2 = mp2::K2TreeGraph::load(basename);
                row.space_bytes = k2.size_bytes_on_disk();
                row.correct = verify_k2(adjacency, k2, cfg.verify_samples, cfg.seed + row.max_level);

                if (row.correct) {
                    const auto queries = mp2::bench::make_queries(n, cfg.ops, cfg.seed);
                    row.degree_us =
                        mp2::bench_cli::bench_degree(k2, queries, cfg.ops, cfg.reps).per_op_mean_us;
                    row.neighbors_us =
                        mp2::bench_cli::bench_neighbors(k2, queries, cfg.ops, cfg.reps).per_op_mean_us;
                }
            } catch (const std::exception &ex) {
                std::cerr << "  max_level=" << level << " failed: " << ex.what() << '\n';
                row.correct = false;
            }

            rows.push_back(row);
            std::cout << "  L=" << level << " space=" << row.space_bytes << " build_ms=" << row.build_ms
                      << " degree_us=" << row.degree_us << " neighbors_us=" << row.neighbors_us
                      << " correct=" << (row.correct ? "yes" : "no") << '\n';
        }

        const auto best_it = std::min_element(
            rows.begin(), rows.end(), [](const TuneRow &a, const TuneRow &b) {
                if (a.correct != b.correct) {
                    return a.correct && !b.correct;
                }
                if (!a.correct) {
                    return false;
                }
                if (a.space_bytes != b.space_bytes) {
                    return a.space_bytes < b.space_bytes;
                }
                return (a.degree_us + a.neighbors_us) < (b.degree_us + b.neighbors_us);
            });

        if (best_it == rows.end() || !best_it->correct) {
            std::cerr << "No valid k2 max_level found for " << graph_name << '\n';
            return 1;
        }

        std::cout << "\nBest max_level=" << best_it->max_level << " (auto=" << auto_level << ")\n";
        std::cout << "  space=" << best_it->space_bytes << " bytes\n";
        std::cout << "  degree=" << best_it->degree_us << " us/op | neighbors=" << best_it->neighbors_us
                  << " us/op\n";

        std::filesystem::create_directories(std::filesystem::path(cfg.out_csv).parent_path());
        std::ofstream tune_out(cfg.out_csv, std::ios::app);
        write_tune_header(tune_out, cfg.out_csv);
        for (const TuneRow &row : rows) {
            tune_out << graph_name << ',' << n << ',' << m << ',' << auto_level << ',' << row.max_level
                     << ',' << row.space_bytes << ',' << row.build_ms << ',' << row.degree_us << ','
                     << row.neighbors_us << ',' << (row.correct ? 1 : 0) << ',' << cfg.ops << ','
                     << cfg.reps << ',' << cfg.seed << '\n';
        }

        std::filesystem::create_directories(std::filesystem::path(cfg.best_csv).parent_path());
        std::ofstream best_out(cfg.best_csv, std::ios::app);
        write_best_header(best_out, cfg.best_csv);
        best_out << graph_name << ',' << n << ',' << m << ',' << auto_level << ',' << best_it->max_level
                 << ',' << best_it->space_bytes << ',' << best_it->degree_us << ',' << best_it->neighbors_us
                 << ',' << best_it->build_ms << '\n';

        std::cout << "\nTune CSV: " << cfg.out_csv << '\n';
        std::cout << "Best CSV: " << cfg.best_csv << '\n';
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "mp2_bench_k2_tune failed: " << ex.what() << '\n';
        return 1;
    }
}
