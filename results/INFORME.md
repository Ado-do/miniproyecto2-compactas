# Informe experimental — Miniproyecto 2

**Integrantes:** Alonso Bustos, Joaquín San Martín  
**Fecha:** julio 2026  
**Datos:** `benchmark_full.csv`, `k2_best_params.csv`, `k2_tune.csv`

## Objetivo

Comparar espacio y tiempo de `degree(v)` y `neighbors(u,v)` en tres representaciones de grafos planares:

1. Lista de adyacencia (implementación propia)
2. k2-tree Basic (`k2tree_basic_v0.1`, `k=2`)
3. Extensión de Turán (`pemb.hpp` / `PembGraph`)

## Metodología

| Parámetro | Valor |
|-----------|-------|
| Consultas por medición | 1000 |
| Repeticiones | 30 |
| Semilla queries | 42 |
| Cronómetro | `std::chrono::high_resolution_clock` |
| Tiempo reportado | promedio del batch / 1000 (µs/op) |

**Hardware:** MacBook Air M4 2025, 16 GB RAM, macOS arm64, Apple Clang, build `RelWithDebInfo`.

**Datasets:** `tiger_map_hawaii.pg`, `planar_embedding1000000.pg`, `worldcitiespop.pg` (1 componente conexa cada uno).

Reproducir:

```bash
./execute_benchmarks.sh k2-tune
./execute_benchmarks.sh full
```

## Parámetros k2-tree Basic

Único parámetro tunable: `max_level` (`k=2` fijo). Barrido `[auto, auto+2]`; criterio: build correcto → menor espacio → menor tiempo.

| Grafo | n | auto | **max_level usado** |
|-------|---|------|---------------------|
| Hawaii | 33,558 | 15 | 15* |
| Planar-1M | 1,000,000 | 19 | 20 |
| World cities | 2,243,467 | 21 | 23 |

\* En `k2_best_params.csv` el tune eligió **16** para Hawaii; `benchmark_full.csv` registró L=15. Re-ejecutar `full` tras `k2-tune` si se requiere consistencia.

## Resultados — espacio (bytes)

| Grafo | Adjacency | k2-tree | pemb | pemb vs adj |
|-------|-----------|---------|------|-------------|
| Hawaii | 1,452,480 | 270,288 | 74,302 | **19.5×** menos |
| Planar-1M | 47,999,856 | 26,561,008 | 2,102,776 | **22.8×** menos |
| World cities | 107,686,400 | 63,624,044 | 4,674,203 | **23.0×** menos |

**Conclusión espacio:** pemb es la representación más compacta; k2-tree intermedio; lista de adyacencia la más costosa en RAM.

## Resultados — tiempo (µs/op, promedio)

### k2-tree (mediciones confiables)

| Grafo | degree | neighbors |
|-------|--------|-----------|
| Hawaii | 9.74 | 0.083 |
| Planar-1M | 146.24 | 0.174 |
| World cities | 234.94 | 0.193 |

`neighbors` en k2-tree es mucho más rápido que `degree` (query de enlace vs recuperar lista completa).

### Adjacency y pemb

Los tiempos reportados (~0.001–0.03 µs/op) están en el **piso de resolución** del cronómetro para operaciones O(1) en caché. No deben interpretarse como literal sub-nanosegundo.

- **Adjacency:** `degree` O(1), `neighbors` búsqueda en lista de grado local.
- **Pemb:** `degree` O(1) vía `getVertexFirst/Last` del embedding `Graph`; `neighbors` escanea aristas del embedding. El espacio medido es del `pemb` compacto; las consultas no usan la API nativa `first/next` de `pemb`.

## Conclusiones

1. **Compactación:** pemb reduce espacio ~20× respecto a lista de adyacencia en los tres grafos.
2. **k2-tree:** buen compromiso espacio/tiempo; `neighbors` muy eficiente.
3. **Trade-off:** representaciones compactas ahorran memoria; k2 paga más en `degree` que en `neighbors`.
4. **Limitaciones:** consultas pemb no miden navegación compacta nativa; tiempos adj/pemb en CSV requieren cautela en el análisis.

## Archivos

| Archivo | Descripción |
|---------|-------------|
| `benchmark_full.csv` | Resultados 3×3×2 operaciones |
| `k2_best_params.csv` | Mejor `max_level` por grafo |
| `k2_tune.csv` | Detalle del barrido de parámetros |
