#!/bin/bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"

usage() {
    cat <<'EOF'
Usage: ./execute_benchmarks.sh <command>

Commands:
  quick     Smoke + benchmark rapido (100 ops x 3 reps, Hawaii)
  k2-tune   Barrido max_level k2-tree Basic en los 3 grafos
  full      Benchmark completo enunciado (1000 ops x 30 reps)
  all       k2-tune + full (recomendado antes de informe)

Variables de entorno utiles:
  GRAPHS_DIR, OPS, REPS, SEED, OUT, LEVEL_SPAN

Ejemplos:
  ./execute_benchmarks.sh quick
  ./execute_benchmarks.sh k2-tune
  OPS=50 REPS=2 ./execute_benchmarks.sh k2-tune
  ./execute_benchmarks.sh all
EOF
}

cmd="${1:-quick}"

case "$cmd" in
    quick)
        ./scripts/benchmark_quick.sh
        ;;
    k2-tune)
        ./scripts/benchmark_k2_tune.sh
        ;;
    full)
        ./scripts/benchmark_full.sh
        ;;
    all)
        ./scripts/benchmark_k2_tune.sh
        ./scripts/benchmark_full.sh
        ;;
    -h|--help|help)
        usage
        ;;
    *)
        echo "Comando desconocido: $cmd"
        usage
        exit 1
        ;;
esac
