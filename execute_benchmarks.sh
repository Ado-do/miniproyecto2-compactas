#!/bin/bash
set -e

./compile.sh

if [[ -x build/mp2_smoke && -f material-base/datasets/graphs/tiger_map_hawaii.pg ]]; then
    echo "* Smoke test (Hawaii)..."
    ./build/mp2_smoke material-base/datasets/graphs/tiger_map_hawaii.pg
else
    echo "Benchmarks completos pendientes (Fase 5 del roadmap)."
    echo "Ejecuta ./build/mp2_smoke <grafo.pg> para validar el build."
fi
