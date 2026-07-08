# Material base — Miniproyecto 2

Documentación y código de referencia. No es el código de entrega.

| Recurso | Descripción |
|---------|-------------|
| [ENUNCIADO.md](ENUNCIADO.md) | Consigna oficial |
| [K2-TREE.md](K2-TREE.md) | k2-tree (basic + hybrid) |
| [TURAN-PEMB.md](TURAN-PEMB.md) | Extensión de Turán (`pemb.hpp`) |
| [DATASETS.md](DATASETS.md) | Formato UdeC, grafos locales, links |
| [PENDIENTES.md](PENDIENTES.md) | Checklist de lo que falta |
| [ROADMAP.md](ROADMAP.md) | **Guía de trabajo** — fases, tareas y sprints |
| [k2tree_v0.2/](k2tree_v0.2/) | k2-tree hybrid |
| [k2tree_basic_v0.1/](k2tree_basic_v0.1/) | k2-tree basic |
| [sdsl-lite-turan/](sdsl-lite-turan/) | Fork sdsl-lite con `pemb.hpp` |
| [udec-code/](udec-code/) | Código read/write grafos UdeC |
| [datasets/graphs/](datasets/graphs/) | 3 grafos `.pg` para experimentos |

## Estructura `datasets/`

```
datasets/graphs/     ← únicos archivos de datos (3 grafos .pg)
```

## Grafos listos (3/3)

| Grafo | Archivo |
|-------|---------|
| Planar-1M | `datasets/graphs/planar_embedding1000000.pg` |
| World cities | `datasets/graphs/worldcitiespop.pg` |
| Tiger Hawaii | `datasets/graphs/tiger_map_hawaii.pg` |

## Decisiones de proyecto

| Tema | Elección |
|------|----------|
| k2-tree | **Basic** (`k2tree_basic_v0.1`) — ver [ROADMAP.md](ROADMAP.md) |
| Quick win | Hybrid en Hawaii si da el tiempo (Opción C) |
