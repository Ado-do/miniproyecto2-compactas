#!/bin/bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

GRAPHS_DIR="${GRAPHS_DIR:-material-base/datasets/graphs}"
OPS="${OPS:-200}"
REPS="${REPS:-3}"
SEED="${SEED:-42}"
LEVEL_SPAN="${LEVEL_SPAN:-4}"
OUT="${OUT:-results/k2_tune.csv}"
BEST="${BEST:-results/k2_best_params.csv}"

HAWAII="$GRAPHS_DIR/tiger_map_hawaii.pg"
PLANAR="$GRAPHS_DIR/planar_embedding1000000.pg"
WORLD="$GRAPHS_DIR/worldcitiespop.pg"

./compile.sh

rm -f "$OUT" "$BEST"

run_tune() {
    local graph="$1"
    if [[ ! -f "$graph" ]]; then
        echo "SKIP (no existe): $graph"
        return 0
    fi
    echo "=== k2 tune: $(basename "$graph") ==="
    ./build/mp2_bench_k2_tune "$graph" \
        --ops "$OPS" \
        --reps "$REPS" \
        --seed "$SEED" \
        --level-span "$LEVEL_SPAN" \
        --out "$OUT" \
        --best "$BEST"
}

run_tune "$HAWAII"
run_tune "$PLANAR"
run_tune "$WORLD"

echo "Tune detail: $OUT"
echo "Best params: $BEST"
