#include "k2_tree_bridge.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "kTree.h"

struct mp2_k2_tree {
    MREP *rep;
    char *basename;
};

static uint32_t compute_max_level(uint32_t nodes) {
    if (nodes <= 1) {
        return 0;
    }

    const double levels = log((double)nodes) / log((double)K);
    uint32_t max_level = (uint32_t)floor(levels);
    if (floor(levels) == levels) {
        --max_level;
    }
    return max_level;
}

static mp2_k2_tree *wrap_loaded(MREP *rep, const char *basename) {
    mp2_k2_tree *tree = (mp2_k2_tree *)malloc(sizeof(mp2_k2_tree));
    if (tree == NULL) {
        destroyRepresentation(rep);
        return NULL;
    }

    tree->rep = rep;
    tree->basename = (char *)malloc(strlen(basename) + 1);
    if (tree->basename == NULL) {
        destroyRepresentation(rep);
        free(tree);
        return NULL;
    }
    strcpy(tree->basename, basename);
    return tree;
}

mp2_k2_tree *mp2_k2_build_from_arcs(const uint32_t *xedges, const uint32_t *yedges, uint32_t nodes,
                                    uint64_t arcs, const char *basename) {
    const uint32_t max_level = compute_max_level(nodes);
    MREP *built = compactCreateKTree((uint *)xedges, (uint *)yedges, nodes, (ulong)arcs, max_level);
    if (built == NULL) {
        return NULL;
    }

    saveRepresentation(built, (char *)basename);
    destroyRepresentation(built);

    MREP *loaded = loadRepresentation((char *)basename);
    if (loaded == NULL) {
        return NULL;
    }

    return wrap_loaded(loaded, basename);
}

mp2_k2_tree *mp2_k2_load(const char *basename) {
    MREP *loaded = loadRepresentation((char *)basename);
    if (loaded == NULL) {
        return NULL;
    }
    return wrap_loaded(loaded, basename);
}

void mp2_k2_destroy(mp2_k2_tree *tree) {
    if (tree == NULL) {
        return;
    }
    if (tree->rep != NULL) {
        destroyRepresentation(tree->rep);
    }
    free(tree->basename);
    free(tree);
}

uint32_t mp2_k2_vertices(const mp2_k2_tree *tree) {
    return tree->rep->numberOfNodes;
}

uint64_t mp2_k2_arcs(const mp2_k2_tree *tree) {
    return (uint64_t)tree->rep->numberOfEdges;
}

uint32_t mp2_k2_degree(const mp2_k2_tree *tree, uint32_t vertex) {
    const uint *list = compact2AdjacencyList(tree->rep, (int)vertex);
    return list[0];
}

int mp2_k2_neighbors(const mp2_k2_tree *tree, uint32_t u, uint32_t v) {
    return compact2CheckLinkQuery(tree->rep, u, v) != 0;
}
