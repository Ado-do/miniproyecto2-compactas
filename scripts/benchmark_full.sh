#!/bin/bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

GRAPHS_DIR="${GRAPHS_DIR:-material-base/datasets/graphs}"
OPS="${OPS:-1000}"
REPS="${REPS:-30}"
SEED="${SEED:-42}"
OUT="${OUT:-results/benchmark_full.csv}"
BEST_PARAMS="${BEST_PARAMS:-results/k2_best_params.csv}"

HAWAII="$GRAPHS_DIR/tiger_map_hawaii.pg"
PLANAR="$GRAPHS_DIR/planar_embedding1000000.pg"
WORLD="$GRAPHS_DIR/worldcitiespop.pg"

read_k2_level() {
    local graph_file="$1"
    local level=""
    if [[ -f "$BEST_PARAMS" ]]; then
        level=$(awk -F, -v g="$(basename "$graph_file")" '
            NR > 1 && $1 == g { print $5; exit }
        ' "$BEST_PARAMS")
    fi
    if [[ -n "$level" ]]; then
        echo "--k2-max-level $level"
    fi
}

./compile.sh

rm -f "$OUT"

run_graph() {
    local graph="$1"
    shift
    if [[ ! -f "$graph" ]]; then
        echo "SKIP (no existe): $graph"
        return 0
    fi
    echo "=== Benchmark: $(basename "$graph") ==="
    ./build/mp2_bench "$graph" --ops "$OPS" --reps "$REPS" --seed "$SEED" --out "$OUT" "$@"
}

run_graph "$HAWAII" $(read_k2_level "$HAWAII")
run_graph "$PLANAR" $(read_k2_level "$PLANAR")
run_graph "$WORLD" $(read_k2_level "$WORLD")

echo "Resultados en $OUT"
