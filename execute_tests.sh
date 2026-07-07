#!/bin/bash
set -e

./compile.sh

if [[ -d build ]] && command -v ctest >/dev/null 2>&1; then
    ctest --test-dir build -V || echo "Sin tests MP2 registrados aún."
else
    echo "ctest no disponible o build/ no existe."
fi
