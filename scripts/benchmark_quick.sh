#!/bin/bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

GRAPHS_DIR="${GRAPHS_DIR:-material-base/datasets/graphs}"
OPS="${OPS:-100}"
REPS="${REPS:-3}"
SEED="${SEED:-42}"
OUT="${OUT:-results/quick.csv}"
BEST_PARAMS="${BEST_PARAMS:-results/k2_best_params.csv}"

HAWAII="$GRAPHS_DIR/tiger_map_hawaii.pg"

read_k2_level() {
    local graph_file="$1"
    if [[ -f "$BEST_PARAMS" ]]; then
        awk -F, -v g="$(basename "$graph_file")" 'NR > 1 && $1 == g { print $5; exit }' "$BEST_PARAMS"
    fi
}

./compile.sh

if [[ ! -f "$HAWAII" ]]; then
    echo "Dataset no encontrado: $HAWAII"
    exit 1
fi

rm -f "$OUT"
./build/mp2_smoke "$HAWAII"

K2_LEVEL="$(read_k2_level "$HAWAII")"
BENCH_ARGS=(--ops "$OPS" --reps "$REPS" --seed "$SEED" --out "$OUT")
if [[ -n "${K2_LEVEL:-}" ]]; then
    BENCH_ARGS+=(--k2-max-level "$K2_LEVEL")
fi
./build/mp2_bench "$HAWAII" "${BENCH_ARGS[@]}"
