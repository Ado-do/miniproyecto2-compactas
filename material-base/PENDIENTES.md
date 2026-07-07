# Pendientes — material que falta pasar

Checklist de lo que **ya tenemos** vs lo que **aún falta**.  
**Roadmap detallado de implementación:** [ROADMAP.md](ROADMAP.md)

## Ya recibido

- [x] Enunciado del mini-proyecto → [ENUNCIADO.md](ENUNCIADO.md)
- [x] Roadmap de tareas → [ROADMAP.md](ROADMAP.md)
- [x] Documentación k2-tree → [K2-TREE.md](K2-TREE.md)
- [x] Implementación k2-tree hybrid → [k2tree_v0.2/](k2tree_v0.2/)
- [x] Implementación k2-tree basic → [k2tree_basic_v0.1/](k2tree_basic_v0.1/)
- [x] Extensión de Turán → [sdsl-lite-turan/](sdsl-lite-turan/) + [TURAN-PEMB.md](TURAN-PEMB.md)
- [x] Formato datasets → [DATASETS.md](DATASETS.md)
- [x] Código read/write grafos UdeC → [udec-code/](udec-code/)
- [x] 3 grafos `.pg` → [datasets/graphs/](datasets/graphs/)
- [x] Template/scaffold del curso (CMake heredado de miniproyecto 1 — hay que adaptar)

---

## Falta implementar (código)

Ver fases en [ROADMAP.md](ROADMAP.md). Resumen:

- [x] **Fase 0** — k2-tree Basic, CMake unificado, scaffold MP1 eliminado (queda 0.4 integrantes)
- [ ] **Fase 1** — Parser `.pg`, componentes conexas, conversor k2-tree
- [ ] **Fase 2** — **Lista de adyacencia propia** (implementación nuestra)
- [ ] **Fase 3** — Integración k2-tree **Basic** (+ quick win hybrid opcional)
- [ ] **Fase 4** — Integración `pemb.hpp`
- [ ] **Fase 5** — Framework benchmarks (1000 ops, 30 repeticiones)
- [ ] **Fase 6** — Experimentos completos en 3 grafos
- [ ] **Fase 7** — Informe PDF
- [ ] **Fase 8** — ZIP de entrega + README replicable

---

## Falta pasar (de tu parte)

- [x] **Hardware de referencia** — MacBook Air M4 2025, 16 GB RAM, macOS Tahoe 26.5.1 arm64
- [x] **Decisión k2-tree** — **Basic** principal; hybrid en Hawaii como quick win opcional
- [ ] **Confirmar integrantes** del grupo

### Opcional (solo informe)

- [ ] Apuntes/slides de clase — alinear redacción teórica con lo visto en clase (no bloquea código)
- [ ] Plantilla de informe del curso (si existe)

### Opcional

- [ ] Logs de compilación / parches ya probados
- [ ] Ejemplos de informes anteriores
- [ ] Dudas resueltas en foro del curso

---

## Próximo paso

**Sprint 1:** Fase 1 — particionar Hawaii en 17 componentes → [ROADMAP.md](ROADMAP.md)
