#pragma once

#include "planar_graph.hpp"

#include <sdsl/pemb.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Graph;

namespace mp2 {

class PembGraph {
public:
    static PembGraph from_pg_file(const std::string &path);
    static PembGraph from_pg_graph(const PgGraph &pg);
    static PembGraph from_component(const ComponentPgGraph &component);

    PembGraph(const PembGraph &) = delete;
    PembGraph &operator=(const PembGraph &) = delete;
    PembGraph(PembGraph &&) noexcept = default;
    PembGraph &operator=(PembGraph &&) noexcept = default;
    ~PembGraph();

    std::size_t vertices() const;
    std::size_t edges() const;
    std::size_t degree(std::uint32_t v) const;
    bool neighbors(std::uint32_t u, std::uint32_t v) const;
    std::size_t size_bytes() const;

private:
    PembGraph(std::unique_ptr<Graph> graph, std::unique_ptr<sdsl::pemb<>> pemb);

    std::unique_ptr<Graph> graph_;
    mutable std::unique_ptr<sdsl::pemb<>> pemb_;
};

std::vector<PembGraph> read_pg_pemb_components(const std::string &path);

} // namespace mp2
