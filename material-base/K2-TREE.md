# k2-tree

The k2-tree is a Web graph representation based on a compact tree structure that takes advantage of large empty areas of the adjacency matrix of the graph. The representation not only retrieves successors and predecessors in symmetric fashion, but also it is particularly efficient to check for specific links between nodes, or between ranges of nodes, or to list the links between ranges. The representation is also of general interest and can be used to compress other kinds of graphs and data structures.

The original idea was published in SPIRE'09. More details can be found in Susana Ladra's PhD thesis. There is also a journal article, published in *Information Systems*, which contains the latest version of the approach.

In this page we provide two versions of the source code for the k2-tree. They are implemented in C.

- **Hybrid approach** — corresponds to the version used in the journal article, with different k values and compression of leaf submatrices. The compression generates four different files (`.tr`, `.lv`, `.cil`, `.voc`) that composed the final compressed representation.
- **Basic approach** — uses k=2 for all the levels, only uses one bitmap for representing the whole tree, and the construction process diverges (uses a counting-sort strategy to compute the bitmaps for each level). In this case only a output file is generated (`.kt`).

## Implementaciones locales

| Versión | Ruta |
|---------|------|
| Hybrid k2-tree (con vocabulario de hojas) | [`k2tree_v0.2/`](k2tree_v0.2/) |
| Basic k2-tree | [`k2tree_basic_v0.1/`](k2tree_basic_v0.1/) |

Fuente original: [https://lbd.udc.es/research/k2tree/](https://lbd.udc.es/research/k2tree/)

## Usage

### Comprimir un grafo (hybrid)

```bash
./build_tree <GRAPH> <name> <K1> <K2> [<max level K1>] <S>
./compress_leaves <name> <hash size>
```

El segundo parámetro de `compress_leaves` debe establecerse según el tamaño y propiedades del grafo y la RAM de la máquina. Por ejemplo, un valor posible es `4000000`.

### Formato binario del grafo (`<GRAPH>`)

Consiste en:

1. Un entero con el número de nodos
2. Un `double` con el número de aristas
3. La lista de adyacencia de cada nodo, marcando con un número negativo el inicio de la siguiente lista

Es decir: `-1`, luego los vecinos del primer nodo; `-2`, la lista de vecinos del segundo nodo; y así sucesivamente.

**Ejemplo** — primeros enteros del grafo `cnr-2000` en formato binario:

```
325557 3216152 0 -1 2 343 344 345 346 347 348 349 350 351 352 211285 223143 -2 3 4 5 320 -3 211285 223143 -4 -5 318 -6 -7 118 219 297 -8 7 19 219 286 297 -9 7
```

> **Nota:** Los números del archivo empiezan en 1, pero la implementación considera la numeración desde 0. Si se pide la lista de sucesores del nodo 6, retorna `117, 218, 296`. La lista de sucesores del nodo 5 es nula.

### Parámetros de `build_tree`

| Parámetro | Descripción |
|-----------|-------------|
| `<name>` | Basename de los archivos de salida. Genera 4 archivos: `name.tr`, `name.lv`, `name.cil`, `name.voc` (representación comprimida final, incluye estructuras extra para rank sobre bits). También genera un archivo temporal `name.il` (muy grande) que puede borrarse tras `./compress_leaves`. |
| `<K1> <K2>` | Valores k para el enfoque hybrid. Deben ser **4** y **2** respectivamente. |
| `<max level K1>` | Obligatorio. Recomendado: **5** para grafos pequeños, **6** para los más grandes. |
| `<S>` | Acelera la construcción (o permite construir grafos más grandes con poca RAM). Valor sugerido: **22** si el grafo es grande. |

### Operaciones sobre el grafo comprimido

Una vez construido el grafo comprimido, se opera con `./use_tree`.

| Comando | Descripción |
|---------|-------------|
| `./rebuild_tree` | Reconstruye el grafo binario original |
| `./checkrebuild_tree` | Reconstruye verificando cada enlace (extremadamente lento; útil para testear) |
| `./rangerebuild_tree` | Reconstruye usando range queries (mucho más rápido) |
| `./invrebuild_tree` | Reconstruye el grafo transpuesto |
| `./test_tree` | Testea velocidad de sucesores |
| `./revtest_tree` | Testea velocidad de predecesores |

Para `test_tree` y `revtest_tree` se pasa como parámetro un archivo binario donde el primer entero indica el número de queries a procesar y los siguientes indican el id del nodo del que se quieren recuperar sucesores o predecesores.

> **Nota:** Para queries k2-tree, los ids de nodos van de **0** a **n-1**.

## Acknowledgements

If you use this source code, please quote the journal article as follows:

```bibtex
@ARTICLE {
        BLNis13.2,
	AUTHOR = "N. Brisaboa and S. Ladra and G. Navarro",
	TITLE = "Compact Representation of Web Graphs with Extended 
		Functionality",
        JOURNAL = {Information Systems},
	VOLUME = 39,
	NUMBER = 1,
	PAGES = "152--174",
        YEAR = 2014
        }
```

## About us

We are two researchers from the University of A Coruña (Spain) and one from the University of Chile (Chile).

- **Nieves R. Brisaboa** (PhD). Database Lab. UDC
- **Susana Ladra** (PhD). Database Lab. UDC
- **Gonzalo Navarro** (PhD). Computer Science Department. UChile

This work was supported in part by MICINN (PGE and FEDER) grants TIN2009-14560-C03-02 and Xunta de Galicia (co-funded with FEDER) grants 2010/17 and CN 2012/211, and by Millennium Nucleus Information and Coordination in Networks ICM/FIC P10-024F.
