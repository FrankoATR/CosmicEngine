# WandAllegroEngine

## Fecha efectiva

**20 de noviembre de 2024**

## Descripción general

En esta etapa consolidé varias piezas base del runtime para que objetos, timers e input trabajen con reglas más consistentes. El cambio principal ya no está solo en disponer de temporización reutilizable, sino en que el modelo base de objetos empieza a exponer movimiento parametrizado por dirección, velocidad y duración, mientras la temporización corrige mejor su relación con el estado de pausa.

Sobre esa misma base endurecí el tratamiento de objetos que salen del área válida del grid, ajusté detalles de limpieza entre managers y terminé de alinear parte de la API de entrada con tipos fuertemente tipados del framework.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- ampliar el modelo base de objetos con movimiento utilitario reutilizable;
- refinar la semántica temporal de `GameTimer` cuando el timer nace en pausa;
- endurecer ciclo de vida, limpieza y tipado de entrada dentro del runtime.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Models/`, donde `GameObject` amplía de forma visible su contrato con dirección, velocidad, rotación y helpers de movimiento basados en tiempo.
- `src/WandAllegroEngine/Models/GameTimer.*`, donde la temporización afina su comportamiento de arranque en pausa y expone lectura de tiempo transcurrido.
- `src/WandAllegroEngine/Collisions/`, `src/WandAllegroEngine/Managers/` e `src/WandAllegroEngine/Interfaces/`, donde se endurecen reglas de destrucción fuera del grid, limpieza de recursos y uso de `enum class` para eventos de entrada.

## Arquitectura o sistemas principales

### Movimiento utilitario en GameObject

`GameObject` deja de limitarse a posición y tamaño básicos y pasa a exponer una base de movimiento reutilizable dentro del propio modelo. La ampliación visible incluye:

- `Direction` y `ViewDirection` como vectores de estado;
- `Velocity` y `Rotation` como parte del contrato del objeto;
- `SetDirection()` y `GetDirection()`;
- `SetRotation()` y `GetRotation()`;
- `SetVelocity()` y `GetVelocity()`;
- `UpdatePosition(float Velocity, double DeltaTime)`;
- `ReachPositionInTime(WAND_VEC2 NewPosition, double Duration, double DeltaTime)`;
- `MoveForDirection(WAND_VEC2 NewDirection, double Duration, double DeltaTime)`.

Con ello el framework ya puede encapsular desplazamiento continuo y desplazamiento acotado por duración sin obligar a cada entidad a recalcular siempre su propia lógica vectorial desde cero.

### Temporización más consistente bajo pausa

`GameTimer` conserva la corrección previa de arranque diferido, pero ahora la inicialización de `LastTimeTrigger` ocurre solo cuando el timer deja de estar pausado. Ese ajuste evita que un timer nacido en pausa consuma tiempo lógico antes de activarse realmente.

Además, el modelo expone `GetElapsedTime()`, lo que deja una lectura explícita del punto temporal interno usado por el timer para coordinar su siguiente disparo.

Con esto la infraestructura temporal queda mejor alineada con casos donde el timer se crea antes de empezar a correr, como cooldowns retenidos o movimientos activados bajo una condición posterior.

### Movimiento con duración apoyado en timers

El propio `GameObject` pasa a poder apoyarse en un `MovementTimer` interno para resolver desplazamientos durante una duración fija. La semántica visible es que el modelo puede iniciar un movimiento orientado, mantenerlo activo mientras el timer no termina y cerrarlo cuando la duración se cumple.

Esto no reemplaza todavía toda la lógica específica de entidades, pero sí deja en el nivel base del framework una ruta reutilizable para mover objetos por tiempo en lugar de solo por frame o por entrada directa.

### Endurecimiento del ciclo de vida espacial

`GameGridCollisions` cambia la consecuencia visible de salir del área válida: el objeto deja de volver a la última posición y pasa a destruirse directamente cuando su cuerpo ya no cabe dentro del grid activo.

En paralelo, la verificación de colisión evita procesar pares cuyo objeto padre ya no está vivo. Con ello la capa de colisiones deja una semántica más estricta para objetos fuera de rango y reduce trabajo sobre entidades ya marcadas para destrucción.

### Ajustes de robustez en managers e input

En `BodyManager::Clear()` la grilla activa ahora también se destruye y la referencia se nulifica, evitando que el manager conserve memoria o punteros residuales después de limpiar una escena.

Además, `Definitions.hpp` convierte `KeyEventType` en `enum class`, e `InputManager` adapta sus comparaciones a ese tipado fuerte. Junto con ello se corrigen varias llamadas de Allegro para usar valores nulos y banderas explícitas en lugar de literales heredados.

En `GameScene::Clear()` también se reordena la liberación para vaciar primero música y sonido antes del resto de managers visuales y espaciales.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- el modelo base de objetos ya puede servir como punto común para movimiento dirigido, movimiento por duración y temporización de desplazamientos;
- el runtime maneja con reglas más estrictas la salida del área activa, la limpieza de grillas y el uso tipado de entrada dentro de managers y utilidades visuales.

Con ello dejé una base más útil para movimientos programados, proyectiles, comportamientos dirigidos y limpieza segura de recursos espaciales, con una semántica temporal y de entrada más consistente dentro del motor.

## Dependencias externas visibles

No incorporé dependencias externas visibles nuevas en esta etapa.

## Resumen técnico de la versión

La etapa efectiva al **20 de noviembre de 2024** queda delimitada por estos movimientos:

- amplié `GameObject` con dirección, velocidad, rotación y helpers visibles para movimiento continuo o acotado por duración;
- incorporé soporte interno de movimiento temporal en el propio modelo base mediante `MovementTimer`;
- ajusté `GameTimer` para que su referencia inicial se fije solo al comenzar realmente cuando nace en pausa y añadí `GetElapsedTime()`;
- endurecí `GameGridCollisions` para destruir objetos que salen del área válida y evitar colisiones sobre entidades ya no vivas;
- completé la limpieza de `BodyManager` destruyendo la grilla activa y nulificando su referencia;
- convertí `KeyEventType` en `enum class` y alineé `InputManager` con ese tipado fuerte.

Con esta etapa dejé el framework más preparado para movimiento dirigido, control temporal más preciso y manejo espacial más estricto dentro del runtime.