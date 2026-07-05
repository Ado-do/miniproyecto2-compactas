#!/bin/bash
set -e

echo "* Compilando..."
chmod +x compile.sh execute_benchmarks.sh execute_tests.sh plot.sh
cmake -S . -B build
./compile.sh
