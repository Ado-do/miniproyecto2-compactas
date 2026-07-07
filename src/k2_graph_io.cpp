#include "k2_graph_io.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <limits>
#include <stdexcept>
#include <vector>

#include "graph_io.hpp"

namespace mp2 {

namespace {

constexpr std::uint32_t k2_marker(std::uint32_t vertex) {
    const auto marker = static_cast<std::int32_t>(-(static_cast<std::int32_t>(vertex) + 1));
    return static_cast<std::uint32_t>(marker);
}

std::uint64_t adjacency_arc_count(const AdjacencyList &graph) {
    std::uint64_t arcs = 0;
    for (std::uint32_t v = 0; v < graph.vertices(); ++v) {
        arcs += graph.degree(v);
    }
    return arcs;
}

void write_u32(std::FILE *out, std::uint32_t value) {
    if (std::fwrite(&value, sizeof(value), 1, out) != 1) {
        throw std::runtime_error("failed to write uint32 to k2 binary");
    }
}

void write_u64(std::FILE *out, std::uint64_t value) {
    if (std::fwrite(&value, sizeof(value), 1, out) != 1) {
        throw std::runtime_error("failed to write uint64 to k2 binary");
    }
}

std::uint32_t read_u32(std::FILE *in) {
    std::uint32_t value = 0;
    if (std::fread(&value, sizeof(value), 1, in) != 1) {
        throw std::runtime_error("unexpected end of k2 binary (uint32)");
    }
    return value;
}

std::uint64_t read_u64(std::FILE *in) {
    std::uint64_t value = 0;
    if (std::fread(&value, sizeof(value), 1, in) != 1) {
        throw std::runtime_error("unexpected end of k2 binary (uint64)");
    }
    return value;
}

} // namespace

void write_k2_binary(const AdjacencyList &graph, const std::string &path,
                     const K2BinaryWriteOptions &opts) {
    if (graph.vertices() > static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max())) {
        throw std::out_of_range("graph too large for k2-tree binary format");
    }

    const auto n = static_cast<std::uint32_t>(graph.vertices());
    const std::uint64_t arcs = adjacency_arc_count(graph);

    std::FILE *out = std::fopen(path.c_str(), "wb");
    if (!out) {
        throw std::runtime_error("cannot open k2 binary for writing: " + path);
    }

    write_u32(out, n);
    write_u64(out, arcs);

    for (std::uint32_t v = 0; v < n; ++v) {
        write_u32(out, k2_marker(v));

        std::vector<std::uint32_t> neighbors(graph.adjacent(v).begin(), graph.adjacent(v).end());
        if (opts.sort_neighbors) {
            std::sort(neighbors.begin(), neighbors.end());
        }

        for (const std::uint32_t neighbor : neighbors) {
            if (neighbor >= n) {
                std::fclose(out);
                throw std::out_of_range("neighbor id out of range while writing k2 binary");
            }
            write_u32(out, neighbor + 1);
        }
    }

    if (std::fclose(out) != 0) {
        throw std::runtime_error("failed to close k2 binary: " + path);
    }
}

AdjacencyList read_k2_binary(const std::string &path) {
    std::FILE *in = std::fopen(path.c_str(), "rb");
    if (!in) {
        throw std::runtime_error("cannot open k2 binary for reading: " + path);
    }

    const std::uint32_t n = read_u32(in);
    const std::uint64_t arcs = read_u64(in);

    AdjacencyList graph(n);
    std::uint32_t nodes_read = 0;
    std::uint64_t arcs_read = 0;

    for (std::uint64_t i = 0; i < static_cast<std::uint64_t>(n) + arcs; ++i) {
        const std::uint32_t token = read_u32(in);
        if (static_cast<std::int32_t>(token) < 0) {
            ++nodes_read;
            continue;
        }

        if (nodes_read == 0) {
            std::fclose(in);
            throw std::runtime_error("k2 binary: neighbor before first adjacency marker");
        }

        const std::uint32_t u = nodes_read - 1;
        const std::uint32_t v = token - 1;
        if (u >= n || v >= n) {
            std::fclose(in);
            throw std::out_of_range("k2 binary: vertex id out of range");
        }

        graph.add_edge(u, v);
        ++arcs_read;
    }

    if (std::fgetc(in) != EOF) {
        std::fclose(in);
        throw std::runtime_error("k2 binary: trailing data after adjacency lists");
    }

    std::fclose(in);

    if (nodes_read != n) {
        throw std::runtime_error("k2 binary: expected " + std::to_string(n) +
                                 " adjacency markers, found " + std::to_string(nodes_read));
    }
    if (arcs_read != arcs) {
        throw std::runtime_error("k2 binary: header arc count mismatch");
    }

    return graph;
}

void write_k2_binary(const ComponentGraph &component, const std::string &path,
                     const K2BinaryWriteOptions &opts) {
    write_k2_binary(component.graph, path, opts);
}

std::vector<std::string> write_pg_components_k2_binaries(const std::string &pg_path,
                                                         const std::string &out_dir,
                                                         const std::string &basename) {
    std::error_code ec;
    std::filesystem::create_directories(out_dir, ec);
    if (ec) {
        throw std::runtime_error("cannot create output directory: " + out_dir);
    }

    const auto components = read_pg_components(pg_path);
    std::vector<std::string> outputs;
    outputs.reserve(components.size());

    for (std::size_t i = 0; i < components.size(); ++i) {
        const std::string path = out_dir + "/" + basename + "_c" + std::to_string(i) + ".k2g";
        write_k2_binary(components[i], path);
        outputs.push_back(path);
    }

    return outputs;
}

} // namespace mp2
