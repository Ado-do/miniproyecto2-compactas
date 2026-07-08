#pragma once

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <random>
#include <utility>
#include <vector>

namespace mp2::bench {

struct QueryBatch {
    std::vector<std::uint32_t> degree_vertices;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> neighbor_pairs;
};

inline QueryBatch make_queries(std::size_t n, std::size_t ops, std::uint32_t seed) {
    if (n == 0) {
        throw std::invalid_argument("make_queries: graph has no vertices");
    }

    QueryBatch batch;
    batch.degree_vertices.reserve(ops);
    batch.neighbor_pairs.reserve(ops);

    std::mt19937 rng(seed);
    std::uniform_int_distribution<std::uint32_t> dist(0, static_cast<std::uint32_t>(n - 1));

    for (std::size_t i = 0; i < ops; ++i) {
        batch.degree_vertices.push_back(dist(rng));
        batch.neighbor_pairs.emplace_back(dist(rng), dist(rng));
    }
    return batch;
}

struct TimingStats {
    double batch_mean_us = 0;
    double batch_std_us = 0;
    double per_op_mean_us = 0;
    double per_op_std_us = 0;
};

inline TimingStats time_batch(std::size_t ops, std::size_t reps, const std::function<void()> &run_batch) {
    if (ops == 0 || reps == 0) {
        throw std::invalid_argument("time_batch: ops and reps must be positive");
    }

    std::vector<double> batch_samples;
    batch_samples.reserve(reps);

    for (std::size_t r = 0; r < reps; ++r) {
        const auto t0 = std::chrono::high_resolution_clock::now();
        run_batch();
        const auto t1 = std::chrono::high_resolution_clock::now();
        const double us = std::chrono::duration<double, std::micro>(t1 - t0).count();
        batch_samples.push_back(us);
    }

    double mean = 0;
    for (const double sample : batch_samples) {
        mean += sample;
    }
    mean /= static_cast<double>(reps);

    double variance = 0;
    for (const double sample : batch_samples) {
        const double delta = sample - mean;
        variance += delta * delta;
    }
    const double stddev = reps > 1 ? std::sqrt(variance / static_cast<double>(reps - 1)) : 0;

    const double inv_ops = 1.0 / static_cast<double>(ops);
    return {mean, stddev, mean * inv_ops, stddev * inv_ops};
}

} // namespace mp2::bench
