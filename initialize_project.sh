#!/bin/bash
set -e

echo "* Configurando build..."
chmod +x compile.sh execute_benchmarks.sh execute_tests.sh plot.sh scripts/*.sh

if [[ ! -d material-base/sdsl-lite-turan/include ]]; then
    echo "* Descargando implementación de Extensión de Turan del profesor..."
    git clone --depth 1 https://github.com/jfuentess/sdsl-lite.git material-base/sdsl-lite-turan
    git -C material-base/sdsl-lite-turan submodule update --init --recursive
    echo "* Aplicando parche a louds_tree.hpp que impide la compilación..."
    sed -i 's/tree\.m_select1/tree.m_bv_select1/g' material-base/sdsl-lite-turan/include/sdsl/louds_tree.hpp
    sed -i 's/tree\.m_select0/tree.m_bv_select0/g' material-base/sdsl-lite-turan/include/sdsl/louds_tree.hpp
fi

if [ ! -f material-base/datasets/graphs/tiger_map_hawaii.pg ]; then
    echo "* Descargando datasets..."
    urls=( \
        "https://www.inf.udec.cl/~jfuentess/datasets/files/g/planar1M.tar.gz" \
        "https://www.inf.udec.cl/~jfuentess/datasets/files/g/tiger_map_hawaii.pg.tar.gz" \
        "https://www.inf.udec.cl/~jfuentess/datasets/files/g/worldcities.tar.gz"
    )
    path="material-base/datasets/graphs"
    mkdir -p $path
    for url in "${urls[@]}"; do
        dataset=${url##*/}
        wget -P "$path" -q --show-progress "$url"
        tar xzf "$path/$dataset" -C "$path"
    done
fi

echo "* Compilando..."
cmake -S . -B build
./compile.sh

echo "* Build listo. Prueba: ./build/mp2_smoke material-base/datasets/graphs/tiger_map_hawaii.pg"
