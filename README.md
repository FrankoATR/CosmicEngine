# WandAllegroEngine

## Fecha efectiva

**26 de diciembre de 2024**

## Descripción general

En esta etapa continué sobre la reorganización previa del repositorio, pero el cambio dominante ya no está en la estructura de carpetas sino en la capacidad del runtime para soportar interacciones más ricas dentro de la escena principal. La base visible del framework ahora expone colisiones con lado de contacto, manejo de múltiples cuerpos por entidad y soporte de entrada por mouse integrado al mismo esquema de consulta que ya usaba teclado.

Sobre esa base también amplié el juego de ejemplo para que deje de ser solo una escena lineal mínima y pase a manejar múltiples mapas, intentos, reaparición, fin de nivel y objetos interactivos como orbes. El resultado visible de esta etapa es un template que ya no solo compila y organiza mejor el engine, sino que además demuestra un flujo jugable más completo apoyado directamente por el runtime.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- enriquecer el modelo de colisión para distinguir lados de contacto y permitir respuestas más específicas por entidad;
- ampliar la infraestructura de entrada para combinar teclado y mouse bajo una misma API del engine;
- utilizar esas bases para construir un flujo de escena más completo con mapas, intentos, reaparición y objetos interactivos.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `include/WandEngine/Interfaces/Definitions.hpp`, donde aparecen nuevas enumeraciones visibles para `CollisionSide` y `CollisionType`, abriendo una semántica más explícita para resolver contactos y preparar estrategias de colisión.
- `include/WandEngine/Models/GameBodyObject.*` y `include/WandEngine/Managers/BodyManager.*`, donde el runtime consolida la noción de cuerpos múltiples, callbacks de colisión con información de lado y búsqueda de cuerpos por objeto padre.
- `include/WandEngine/Managers/InputManager.*`, donde la gestión de mouse ya queda integrada mediante estados `down`, `up` y sostenido, al mismo nivel de consulta que teclado.
- `src/Entities/`, donde se incorporan `Orb` y `EndLevel` y se reajustan `Player`, `Ground`, `Spike`, `SolidBlock` y `Background` para responder a un flujo jugable más amplio.
- `src/Scenes/MainScene.*`, donde la escena principal pasa a manejar varios mapas, intentos, temporizador de respawn, reinicio manual y finalización de nivel.
- `CMakeLists.txt`, donde también se actualiza la composición del ejecutable para incluir las nuevas entidades y ajustar el nombre final del binario a `GeometryClone`.

## Arquitectura o sistemas principales

### Colisiones con semántica de lado

La base visible del framework deja de tratar la colisión solo como un encuentro binario entre cuerpos y pasa a exponer también el lado del contacto mediante `CollisionSide`. Esa información se propaga desde el cuerpo físico hacia la entidad propietaria, lo que permite que cada objeto reaccione distinto si el impacto ocurrió por abajo, por arriba o por un lateral.

En esta etapa esa semántica ya se usa de forma concreta para:

- estabilizar al jugador cuando cae sobre `Ground` o sobre la cara superior de un `SolidBlock`;
- distinguir colisiones letales con `Spike`;
- reservar un segundo cuerpo interno del jugador para detectar casos más estrictos de impacto y destrucción.

Además, `Definitions.hpp` incorpora `CollisionType` con valores para `Grid` y `QuadTree`, dejando visible una dirección de extensión futura aunque la integración efectiva siga apoyándose en `GameGridCollisions`.

### Múltiples cuerpos por entidad

`GameBodyObject` y `BodyManager` refuerzan el modelo de cuerpos físicos del runtime. A partir de esta etapa, una entidad puede apoyarse en más de un cuerpo simultáneo y cada uno puede disparar su propia respuesta de colisión.

La aplicación más clara de esta base aparece en `Player`, que pasa a usar:

- un cuerpo principal para resolver piso, bloques, pinchos y orbes;
- un cuerpo secundario interno para endurecer contactos y detectar ciertas colisiones destructivas.

Para soportar esto, `BodyManager` ya mantiene identificadores de cuerpos, eliminación diferida por estado de vida y una búsqueda por objeto padre mediante `FindAllByParent()`.

### Entrada unificada para teclado y mouse

`InputManager` amplía su contrato y ya no se limita al teclado. En esta etapa registra eventos de mouse, mantiene estados `down`, `up` y sostenido para botones y expone `IsMouseButtonPressed()` con la misma semántica de consulta usada por `IsKeyPressed()`.

Ese cambio permite que la lógica del jugador y de la escena ya no dependa solo de `SPACE`, sino también de clic derecho u otros botones del mouse bajo una API común del engine.

### Escena principal con progreso, reaparición y depuración

`MainScene` deja de ser solo una carga fija de objetos y pasa a coordinar un ciclo más completo de juego. La escena visible ahora maneja:

- selección entre varios mapas internos;
- conteo de intentos;
- temporizador de respawn;
- avance al siguiente nivel;
- reinicio manual con teclado;
- visualización opcional de cuerpos de colisión.

En paralelo, la UI se usa para mostrar el estado de intentos y el cierre del juego, mientras la cámara sigue al jugador dentro del mapa activo.

### Objetos interactivos del ejemplo

El juego de ejemplo incorpora dos tipos nuevos de entidad que demuestran el uso del runtime ampliado:

- `Orb`, que responde al contacto con el jugador y permite modificar el salto según su tipo cuando la entrada está siendo utilizada;
- `EndLevel`, que funciona como disparador de fin de nivel, reproduce sonido y fuerza el cierre de la entidad principal para avanzar la escena.

Con ello el template ya muestra un uso más claro del engine para resolver objetos activables, transiciones de nivel y feedback audiovisual dentro de una escena continua.

### Ajustes visibles del build

La configuración de `CMakeLists.txt` se actualiza para reflejar esta etapa del proyecto. El ejecutable pasa a llamarse `GeometryClone` y el target incorpora de forma explícita las nuevas entidades `Orb` y `EndLevel` junto al resto de archivos del juego y del engine.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- el jugador ya combina dos cuerpos de colisión, temporizadores y entrada mixta de teclado y mouse para resolver movimiento, salto y muerte dentro del mapa;
- la escena principal ya usa timers, UI, audio, colisiones y reemplazo de escena para implementar intentos, reaparición, cambio de nivel y finalización del flujo.

Con ello dejé una base más útil para construir prototipos donde el engine no solo dibuja y actualiza objetos, sino que además coordina interacciones más precisas entre input, colisión, audio, UI y transición entre escenas.

## Dependencias externas visibles

No aparecen dependencias externas nuevas respecto a la etapa anterior. Se mantiene la construcción con CMake y el enlace con la biblioteca monolítica de Allegro, además de `sqlite3`, `ole32` y `uuid` definidos en la configuración del proyecto.

## Resumen técnico de la versión

La etapa efectiva al **26 de diciembre de 2024** queda delimitada por estos movimientos:

- amplié la semántica visible de colisión con `CollisionSide` y la preparación de estrategias mediante `CollisionType`;
- reforcé `GameBodyObject` y `BodyManager` para soportar múltiples cuerpos por entidad y callbacks de colisión más expresivos;
- extendí `InputManager` para manejar botones de mouse con la misma lógica de consulta usada por teclado;
- expandí `MainScene` para manejar múltiples niveles, intentos, respawn, reinicio manual y finalización del recorrido;
- incorporé `Orb` y `EndLevel` como entidades nuevas del ejemplo para demostrar interacciones activables y transición de nivel;
- ajusté `CMakeLists.txt` para integrar las nuevas fuentes del juego y renombrar el ejecutable a `GeometryClone`.

Con esta etapa dejé el template en una versión donde la reorganización previa del proyecto ya se traduce en una demostración más rica de colisiones, entrada y flujo de escena dentro del framework.