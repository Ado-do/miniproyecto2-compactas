#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mp2_k2_tree mp2_k2_tree;

/// Returns the default max_level used by the Basic k2-tree (k=2).
uint32_t mp2_k2_auto_max_level(uint32_t nodes);

mp2_k2_tree *mp2_k2_build_from_arcs(const uint32_t *xedges, const uint32_t *yedges,
                                    uint32_t nodes, uint64_t arcs, const char *basename);
/// Pass max_level_override < 0 to use mp2_k2_auto_max_level(nodes).
mp2_k2_tree *mp2_k2_build_from_arcs_level(const uint32_t *xedges, const uint32_t *yedges,
                                          uint32_t nodes, uint64_t arcs, const char *basename,
                                          int32_t max_level_override);
mp2_k2_tree *mp2_k2_load(const char *basename);
void mp2_k2_destroy(mp2_k2_tree *tree);

uint32_t mp2_k2_vertices(const mp2_k2_tree *tree);
uint64_t mp2_k2_arcs(const mp2_k2_tree *tree);
uint32_t mp2_k2_max_level(const mp2_k2_tree *tree);
uint32_t mp2_k2_degree(const mp2_k2_tree *tree, uint32_t vertex);
int mp2_k2_neighbors(const mp2_k2_tree *tree, uint32_t u, uint32_t v);

#ifdef __cplusplus
}
#endif
