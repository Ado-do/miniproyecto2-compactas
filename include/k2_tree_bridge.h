#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mp2_k2_tree mp2_k2_tree;

mp2_k2_tree *mp2_k2_build_from_arcs(const uint32_t *xedges, const uint32_t *yedges,
                                    uint32_t nodes, uint64_t arcs, const char *basename);
mp2_k2_tree *mp2_k2_load(const char *basename);
void mp2_k2_destroy(mp2_k2_tree *tree);

uint32_t mp2_k2_vertices(const mp2_k2_tree *tree);
uint64_t mp2_k2_arcs(const mp2_k2_tree *tree);
uint32_t mp2_k2_degree(const mp2_k2_tree *tree, uint32_t vertex);
int mp2_k2_neighbors(const mp2_k2_tree *tree, uint32_t u, uint32_t v);

#ifdef __cplusplus
}
#endif
