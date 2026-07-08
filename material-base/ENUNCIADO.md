# Grafos planares compactos

## Objetivo

El objetivo principal de este mini-proyecto es que experimenten el potencial que tienen representaciones compactas de grafos, en particular, para el caso de grafo planares. Respecto a implementaciones clásicas, como listas de adyacencia, las representaciones compactas pueden conseguir espacios varias veces menores. Es su oportunidad de medirlo.

## Lo que se pide hacer

En clases, vimos algunas representaciones compactas de grafos generales y de grafos planares. Para el caso de grafos generales, revisamos el k2-tree, mientras que para grafos planares vimos la extensión de Turán. Implementaciones funcionales de esas 2 representaciones pueden ser encontradas en los siguientes enlaces:

- **k2-tree:** [https://lbd.udc.es/research/k2tree/](https://lbd.udc.es/research/k2tree/). Encontrarán 2 versiones. Son libres de usar la que quieran. Se darán cuenta que cada versión tiene parámetros de configuración. Deberán usar los parámetros que mejoren sus resultados. Dichos parámetros deben ser reportados en el informe.
- **Extensión de Turán:** En [https://github.com/jfuentess/sdsl-lite](https://github.com/jfuentess/sdsl-lite), encontrarán una implementación llamada `pemb.hpp`. Pueden ver cómo usarla en la sección final del archivo Readme principal, bajo el subtítulo "Fast and Compact Planar Embeddings".

De manera precisa, se les pide lo siguiente:

- Tomar al menos 3 grafos del sitio [https://www.inf.udec.cl/~jfuentess/datasets/graphs.php](https://www.inf.udec.cl/~jfuentess/datasets/graphs.php) y medir el espacio que consume el k2-tree, la extensión de Turán, y una implementación basada en listas de adyacencia. Nótese que el sitio contiene varios grafos, algunos de los cuales están compuestos por varias componentes conexas. En el caso de grafos con más de una componente, cada componente debe ser almacenada con las representación a medir.
- Deben medir el tiempo promedio de 2 operaciones básicas sobre esos grafos:
  - `degree(v)`: retorna el grado de vértice `v`
  - `neighbors(u,v)`: chequea hay una conexión entre los vértices `u` y `v`

Notarán que ambas operaciones son muy rápidas para medir su tiempo de manera individual. Lo que deben hacer en medir el tiempo total de, por ejemplo, 1000 de esas operaciones. Luego, dividen ese tiempo por 1000 y obtendrán el tiempo promedio por consulta. Recuerden, además, que cada experimento debe ser repetido varias veces, al menos 30, y sacar promedios.

## La entrega consta de

- **Un informe que:**
  - Incluya portada, descripción de la tarea, descripción de las implementaciones, análisis teórico y análisis experimental.
  - Sea claro y esté bien escrito. Un informe difícil de entender será mal evaluado, aunque todo esté bien implementado. Quien revise el documento debe poder entender su solución solo mirando el informe.
  - Esté en formato PDF.
- **Un archivo comprimido** con todos los códigos fuente implementados, incluyendo un readme para replicar los experimentos en caso de ser necesario. El informe debe hacer referencia a ellos y explicar en qué consiste cada uno.

## Consideraciones adicionales

- El mini-proyecto se puede realizar en parejas. Queda a criterio de los estudiantes organizar sus grupos de trabajo.
- Los códigos que se entregan como punto de partida son códigos desarrollados por investigadores/as. Por lo tanto, es normal que no funcionen a la primera. Seguramente deberán adaptar un poco el mecanismo de compilación y/o adaptar el formato de los grafos de entrada. Saber los cambios que deben hacer es parte de lo que se evalúa. Reporten todos los problemas encontrados y su solución en el informe. Pueden consultar con el profesor y ayudante para encontrar soluciones más rápidas.
- La fecha de entrega es el **8 de julio**. Se aceptarán entregas con atraso de hasta máximo 2 días. Por cada día de atraso se aplicará un descuento de 0.5 décimas de la nota final.

## Rúbrica

**Rúbrica proyecto 2-3**

| Criterio | Calificaciones | Pts |
|----------|----------------|-----|
| **Código** | **2 pts** — Bueno (>1.5): El código es correcto y eficiente, además de estar suficientemente documentado. | 2 |
| | **1.5 pts** — Regular (>1.0): El código compila y, en general, los resultados que obtiene son correctos. Hay algunos errores menores, o la documentación no es adecuada, o presenta algunas ineficiencias. | |
| | **1 pt** — Malo (>0.0): El código compila, pero los resultados son incorrectos. Los resultados son parcialmente correctos, pero la documentación es inexistente, o el código es extremadamente ineficiente. | |
| | **0 pts** — No compila: El código proporcionado no compila o no implementa lo solicitado. | |
| **Evaluación experimental** | **3 pts** — Bueno (>2.0): La evaluación experimental está completa, su presentación es adecuada y se discuten adecuadamente las conclusiones que se pueden obtener de la misma. | 3 |
| | **2 pts** — Incompleta (>1.0): La evaluación experimental se presenta adecuadamente, pero no alcanza a abordar todos los experimentos solicitados. | |
| | **1 pt** — Regular (>0.0): La evaluación experimental está incompleta, o su presentación no es adecuada, o no se discuten adecuadamente las conclusiones que se pueden derivar de la misma. | |
| | **0 pts** — Mala: La evaluación experimental está incompleta, o su presentación no es adecuada, o no se discuten adecuadamente las conclusiones que se pueden derivar de la misma. | |
| **Informe** | **2 pts** — Correcto (>1.0): El código es correcto y eficiente, además de estar suficientemente documentado. El informe cubre todos los apartados solicitados, la presentación es correcta y no presenta muchos errores ortográficos. | 2 |
| | **1 pt** — Regular (>0.0): El informe no incluye alguno de los apartados, o la presentación/ortografía no es adecuada. | |
| | **0 pts** — Malo: No se entrega informe o el mismo no cumple los criterios mínimos para ser evaluado. | |

**Puntos totales: 7**
