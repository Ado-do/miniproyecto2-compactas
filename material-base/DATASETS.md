# Datasets — grafos planares UdeC

Fuente: https://www.inf.udec.cl/~jfuentess/datasets/graphs.php

## Formato del archivo

```
<número de vértices>
<número de aristas>
<origen> <destino>
<origen> <destino>
...
```

- Vértices indexados en `[0, n-1]`
- Por cada vértice `v`, sus aristas salientes listadas en **orden counterclockwise**
- Cada arista undirected aparece **dos veces** (u→v y v→u)

Ejemplo (5 vértices, 7 aristas):

```
5
7
0 1
0 2
0 3
0 4
1 0
1 4
1 2
2 0
2 1
3 0
3 4
4 0
4 3
4 1
```

## Código para leer grafos

Del PDF *Read.pdf* → sección **Read and write graphs**.

| Recurso | Link |
|---------|------|
| Página de código | https://www.inf.udec.cl/~jfuentess/datasets/code.php#code_graphs |
| **Descarga directa (este es el link clave)** | https://www.inf.udec.cl/~jfuentess/datasets/code/g/rw_g.tar.gz |
| Copia local extraída | [`udec-code/`](udec-code/) |

Convierte el formato texto del dataset a la representación `Graph` (arrays `VG`/`EG` con `first`, `last`, `src`, `tgt`, `cmp`).

## Datasets locales (descargados)

| Grafo | Archivo |
|-------|---------|
| Planar-1M | [`datasets/graphs/planar_embedding1000000.pg`](datasets/graphs/planar_embedding1000000.pg) |
| World cities | [`datasets/graphs/worldcitiespop.pg`](datasets/graphs/worldcitiespop.pg) |
| Tiger Hawaii | [`datasets/graphs/tiger_map_hawaii.pg`](datasets/graphs/tiger_map_hawaii.pg) |

## Datasets recomendados (≥3 para el informe)

Elegidos por: variedad de tamaño, factibilidad en laptop, y cobertura de componentes conexas.

| # | Dataset | Tamaño | Vértices | Aristas | Componentes | Por qué |
|---|---------|--------|----------|---------|-------------|---------|
| 1 | **Tiger - Hawaii** | 514 KB | 33,558 | 80,882 | **1** en `.pg` | Pequeño; el sitio UdeC menciona 17 regiones TIGER, pero el archivo `.pg` es un solo grafo conexo |
| 2 | **Planar-1M** | 29 MB | 1,000,000 | 2,999,978 | 1 | Sintético grande, benchmark clásico |
| 3 | **World cities** | 68 MB | 2,243,467 | 6,730,395 | 1 | Real-world, tamaño intermedio-grande |

### Links de descarga directa

```text
https://www.inf.udec.cl/~jfuentess/datasets/files/g/tiger_map_hawaii.pg.tar.gz
https://www.inf.udec.cl/~jfuentess/datasets/files/g/planar1M.tar.gz
https://www.inf.udec.cl/~jfuentess/datasets/files/g/worldcities.tar.gz
```

### Alternativas

| Dataset | Tamaño | Notas |
|---------|--------|-------|
| Tiger - Alaska | 1.4 MB | 93K vértices, **50 componentes** — buen 2º dataset pequeño |
| Tiger - 8 States | 73 MB | 4.7M vértices, **1 componente** — sustituto de World cities |
| Tiger U.S.A. | 331 MB | 20M vértices, **98 componentes** — solo si hay RAM/disco |

### No recomendados al inicio

`Planar-5M` en adelante (156 MB–849 MB) y datasets Tiger de ~326 MB — útiles para escalar resultados, no para la primera integración.

## Componentes conexas

El enunciado exige: si un grafo tiene varias componentes **en el archivo `.pg`**, cada una se almacena por separado.

Hawaii en nuestros tests tiene **1** componente conexa (verificado con BFS). Para validar la partición multi-componente, `test_components` incluye un grafo sintético de 2 componentes. Datasets con muchas componentes en `.pg`: Tiger Alaska (50), Tiger U.S.A. (98) — ver sitio UdeC.

## Nota k2-tree

El formato UdeC **no** es el binario de k2-tree. Hay que convertir a:

```
<n> <m> -1 <vecinos nodo 0> -2 <vecinos nodo 1> ...
```

Ver [K2-TREE.md](K2-TREE.md). Numeración k2-tree: **0 … n-1** (compatible con UdeC).
