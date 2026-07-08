# Extensión de Turán — `pemb.hpp`

Implementación en el fork de sdsl-lite de José Fuentes.

| Recurso | Ubicación |
|---------|-----------|
| Repo local | [`sdsl-lite-turan/`](sdsl-lite-turan/) |
| Header principal | [`sdsl-lite-turan/include/sdsl/pemb.hpp`](sdsl-lite-turan/include/sdsl/pemb.hpp) |
| `Graph` + `read_graph_from_file` | [`sdsl-lite-turan/include/complementary/Graph.hpp`](sdsl-lite-turan/include/complementary/Graph.hpp) |
| Repo upstream | https://github.com/jfuentess/sdsl-lite |
| Paper | https://arxiv.org/abs/1610.00130 |

## Uso mínimo

```cpp
#include <sdsl/pemb.hpp>
#include <complementary/Graph.hpp>
#include <complementary/utils.hpp>

using namespace sdsl;
using namespace std;

int main(int argc, char **argv) {
  Graph g = read_graph_from_file(argv[1]);
  pemb<> pe(g);

  cout << "Size in bytes: " << size_in_bytes(pe) << " B" << endl;
  cout << "Degree of vertex 10: " << pe.degree(10) << endl;
}
```

El archivo de entrada debe seguir el formato de grafos planares UdeC (ver [DATASETS.md](DATASETS.md)).

## Operaciones relevantes para el proyecto

- `pe.degree(v)` — grado del vértice
- Para `neighbors(u,v)` hay que revisar la API de `pemb.hpp` (probablemente consulta de adyacencia entre dos vértices)
