# Miniproyecto 2: Grafos Planares Compactos

- Alonso Bustos
- Joaquín San Martín

Comparación de representaciones compactas de grafos planares: **lista de adyacencia** (propia), **k2-tree Basic** y **extensión de Turán** (`pemb.hpp`).

## Estructura del repositorio

```
├── material-base/          # Referencia: docs, terceros, datasets
│   ├── datasets/graphs/    # 3 grafos .pg (no versionados)
│   ├── k2tree_basic_v0.1/  # k2-tree Basic (principal)
│   ├── sdsl-lite-turan/    # pemb.hpp (clonar localmente)
│   └── ROADMAP.md          # Guía de trabajo
├── include/ src/           # Código de entrega (MP2)
├── tests/ benchmarks/      # Tests y benchmarks (por implementar)
├── results/                # CSV y gráficos
└── cds/                    # Representaciones comprimidas generadas (.kt, etc.)
```

## Requisitos

- CMake ≥ 3.15
- Compilador C y C++17 (Clang en macOS arm64 probado)
- Gnuplot (gráficos, fase experimental)

### Dependencias locales (en `material-base/`)

1. **k2-tree Basic** — incluido en el repo
2. **sdsl-lite-turan** — clonar si no existe:
   ```bash
   git clone --depth 1 https://github.com/jfuentess/sdsl-lite.git material-base/sdsl-lite-turan
   cd material-base/sdsl-lite-turan && git submodule update --init --recursive
   ```
3. **Datasets** — descargar según [`material-base/DATASETS.md`](material-base/DATASETS.md) o usar los `.pg` ya en `material-base/datasets/graphs/`

## Build

```bash
chmod +x initialize_project.sh compile.sh execute_benchmarks.sh execute_tests.sh plot.sh
./initialize_project.sh
```

Smoke test (lee un `.pg` y construye lista de adyacencia):

```bash
./build/mp2_smoke material-base/datasets/graphs/tiger_map_hawaii.pg
```

## Experimentos

```bash
./execute_benchmarks.sh   # pendiente Fase 5 del roadmap
./plot.sh
```

## Documentación

- [Roadmap](material-base/ROADMAP.md)
- [Enunciado](material-base/ENUNCIADO.md)
- [Pendientes](material-base/PENDIENTES.md)

## Hardware de referencia (experimentos)

- MacBook Air 13" M4 2025, 16 GB RAM
- macOS Tahoe 26.5.1 arm64, Apple Clang
