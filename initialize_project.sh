#!/bin/bash
set -e

echo "* Configurando build..."
chmod +x compile.sh execute_benchmarks.sh execute_tests.sh plot.sh

if [[ ! -d material-base/sdsl-lite-turan/include ]]; then
    echo "ERROR: falta material-base/sdsl-lite-turan (ver README.md)"
    exit 1
fi

if [[ ! -f material-base/datasets/graphs/tiger_map_hawaii.pg ]]; then
    echo "AVISO: no se encontraron datasets en material-base/datasets/graphs/"
    echo "       Ver material-base/DATASETS.md para descargarlos."
fi

cmake -S . -B build
./compile.sh

echo "* Build listo. Prueba: ./build/mp2_smoke material-base/datasets/graphs/tiger_map_hawaii.pg"
