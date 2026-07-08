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
├── scripts/                # benchmark_quick.sh, benchmark_full.sh, benchmark_k2_tune.sh
├── tests/ benchmarks/      # Tests y benchmarks
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

### Comandos principales

Metodología del enunciado: medir el tiempo de `ops` consultas, dividir por `ops` → tiempo promedio por operación; repetir `reps` veces y promediar.

```bash
./execute_benchmarks.sh quick      # smoke + 100 ops × 3 reps (Hawaii)
./execute_benchmarks.sh k2-tune    # barrido de max_level en los 3 grafos
./execute_benchmarks.sh full       # 1000 ops × 30 reps (enunciado)
./execute_benchmarks.sh all        # k2-tune + full (pipeline recomendado)
./execute_benchmarks.sh help
```

Variables de entorno:

```bash
OPS=1000 REPS=30 ./execute_benchmarks.sh full
LEVEL_SPAN=2 OPS=200 ./execute_benchmarks.sh k2-tune
GRAPHS_DIR=material-base/datasets/graphs OUT=results/mi_run.csv ./execute_benchmarks.sh quick
```

### Binarios de benchmark

```bash
# Comparar las 3 representaciones en un grafo
./build/mp2_bench <grafo.pg> --ops 1000 --reps 30 --seed 42 --out results/benchmark.csv

# Forzar max_level del k2-tree (si ya se tunó)
./build/mp2_bench <grafo.pg> --k2-max-level 16 --out results/benchmark.csv

# Omitir representaciones
./build/mp2_bench <grafo.pg> --skip-pemb --skip-k2

# Barrido de max_level (k2-tree Basic)
./build/mp2_bench_k2_tune <grafo.pg> \
  --level-span 2 --ops 200 --reps 3 \
  --out results/k2_tune.csv --best results/k2_best_params.csv
```

### Salidas generadas

| Archivo | Contenido |
|---------|-----------|
| `results/quick.csv` | Benchmark rápido (Hawaii) |
| `results/benchmark_full.csv` | Benchmark completo 1000×30 |
| `results/k2_tune.csv` | Detalle por cada `max_level` probado |
| `results/k2_best_params.csv` | Mejor `max_level` por grafo (usado por `full`) |

`benchmark_full.sh` aplica automáticamente `k2_best_params.csv` y omite **pemb** en Planar-1M y World cities (las consultas pemb usan el embedding `Graph` en RAM y son órdenes de magnitud más lentas; Hawaii sí incluye pemb).

```bash
./plot.sh   # requiere gnuplot y scripts .p en results/plots/
```

---

### Análisis de parámetros k2-tree Basic

**Versión usada:** `k2tree_basic_v0.1` (enfoque Basic de UdeC).

| Parámetro | Valor | Notas |
|-----------|-------|-------|
| `k` | **2** (fijo) | Constante `K` en `kTree.h`; no configurable en Basic |
| `max_level` | **tunable** | Único parámetro que afecta compresión y rendimiento |

#### Fórmula del valor automático (`auto`)

La librería y nuestro bridge calculan:

```
auto_max_level = floor(log₂(n))
si log₂(n) es entero exacto → auto_max_level -= 1
```

Implementación en `mp2_k2_auto_max_level()` / `buildk2tree.c`.

#### Barrido realizado

Comando: `./execute_benchmarks.sh k2-tune` (rango `[auto − 2, auto + 2]`, verificación de correctitud con 500 muestras aleatorias de `degree`/`neighbors`).

**Hallazgos:**

1. **Niveles por debajo de `auto` suelen fallar** — `compactCreateKTree` llama `exit(-1)` internamente; el tuner aísla cada build en un subproceso (`fork`).
2. **En nuestros 3 grafos, el espacio en disco (`.kt`) fue idéntico** para todos los `max_level` válidos del rango probado.
3. **Criterio de selección** (cuando el espacio empata): menor tiempo promedio de `degree` + `neighbors` (200 ops × 3 reps en tune).
4. **Resultado:** en los tres casos el mejor nivel fue **`auto + 1`**.

#### Parámetros elegidos para experimentos

| Grafo | n | m | auto | **max_level** | espacio `.kt` | degree (µs/op) | neighbors (µs/op) |
|-------|---|---|------|---------------|---------------|----------------|-------------------|
| `tiger_map_hawaii.pg` | 33,558 | 80,882 | 15 | **16** | 270,288 B | 9.87 | 0.11 |
| `planar_embedding1000000.pg` | 1,000,000 | 2,999,978 | 19 | **20** | 26,561,008 B | 163.23 | 0.27 |
| `worldcitiespop.pg` | 2,243,467 | 6,730,395 | 21 | **22** | 63,624,044 B | 244.78 | 0.38 |

Regenerar: `./execute_benchmarks.sh k2-tune` → `results/k2_best_params.csv`

#### Comparación rápida de espacio (Hawaii, tune)

| Representación | Bytes | vs adjacency |
|----------------|-------|--------------|
| Lista adyacencia | 1,634,304 | 1.0× |
| k2-tree Basic (L=16) | 270,288 | **6.0×** menos |
| pemb (Turán) | 74,302 | **22×** menos |

#### Nota sobre k2-tree Hybrid

El enunciado permite también `k2tree_v0.2` (Hybrid: `K1=4`, `K2=2`, `max_level` 5–6, parámetro `S=22`). Quedó como quick win opcional en el roadmap; los experimentos principales usan **Basic** por simplicidad del pipeline (un solo archivo `.kt`).


## Documentación

- [Roadmap](material-base/ROADMAP.md)
- [Enunciado](material-base/ENUNCIADO.md)
- [Pendientes](material-base/PENDIENTES.md)

## Hardware de referencia (experimentos)

- MacBook Air 13" M4 2025, 16 GB RAM
- macOS Tahoe 26.5.1 arm64, Apple Clang
