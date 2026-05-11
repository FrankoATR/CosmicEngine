# WandAllegroEngine

## Fecha efectiva

**2 de noviembre de 2024**

## Descripción general

La etapa efectiva al 2 de noviembre desplaza parte de la infraestructura del motor hacia un esquema de acceso global controlado para escenas y recursos, y acompaña ese cambio con una migración visible de cabeceras desde `.h` hacia `.hpp`. En paralelo, la base del proyecto incorpora un canal explícito de eventos entre objetos y observadores, además de una reorganización menor en el tratamiento de colisiones y en la cobertura espacial del grid.

El resultado inmediato es una separación más marcada entre los managers mantenidos como estado propio de `GameManager` y aquellos que pasan a resolverse como instancias únicas compartidas durante la ejecución.

## Objetivo técnico

Esta revisión queda orientada a tres frentes concretos:

- consolidar la transición de interfaces públicas hacia cabeceras `.hpp`;
- desacoplar la gestión de escenas y recursos del ciclo de vida manual de `GameManager`;
- habilitar una vía simple de notificación entre entidades y consumidores de eventos del runtime.

## Estructura del proyecto

La diferencia visible respecto a la versión anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Managers/`, donde desaparecen `GameSceneManager` y `GameEventManager`, y pasan a coexistir `SceneManager`, `ResourceManager` y `EventManager` bajo nuevas cabeceras `.hpp`.
- `src/WandAllegroEngine/Interfaces/`, que incorpora `GameEvent` como contrato base para receptores de notificaciones.
- `src/Events/`, donde aparece `Logger` como implementación concreta del nuevo mecanismo de eventos.
- `src/Entities/`, `src/Scenes/`, `src/Utilities/`, `src/WandAllegroEngine/Models/` y `src/WandAllegroEngine/Collisions/`, donde la interfaz pública migra desde `.h` hacia `.hpp`.

## Arquitectura o sistemas principales

### Escenas y recursos

`GameManager` deja de administrar punteros propios a `ResourceManager` y `GameSceneManager`. En su lugar:

- el flujo de escenas se resuelve mediante `WandEngine::SceneManager::GetInstance()`;
- la carga y consulta de recursos se canaliza mediante `WandEngine::ResourceManager::GetInstance()`;
- el constructor y la liberación final de `GameManager` reducen su responsabilidad a objetos, cuerpos, ventana y subsistemas de Allegro.

Este ajuste reubica la persistencia de escenas y recursos fuera del ciclo de vida directo del objeto principal del juego.

### Sistema de eventos

Se incorpora una capa mínima de publicación de eventos compuesta por tres piezas visibles:

- `GameEvent`, como interfaz base con `OnNotify(GameObject*, const std::string&)`;
- `EventManager`, como registro singleton de observadores;
- `Logger`, como consumidor concreto agregado desde `MainScene`.

En el estado actual, `LinkObject` emite notificaciones al desplazarse y al destruir enemigos. Con ello aparece una ruta explícita para reaccionar a acciones de gameplay sin incrustar toda la lógica en la entidad emisora.

### Colisiones y partición espacial

La revisión introduce dos variaciones funcionales en este frente:

- `GameBodyManager` expande la grilla principal desde `9 x 12` hacia `10 x 18`, manteniendo celdas de tamaño `100`;
- `GameGridCollisions::Check_cells_collisions()` deja de disparar la notificación inversa sobre el segundo objeto, por lo que el procesamiento efectivo de la colisión queda iniciado solo desde el primer cuerpo evaluado en cada comparación.

La primera modificación amplía el área operativa cubierta por el grid. La segunda altera la simetría observada anteriormente en los callbacks de colisión.

## Escenas y flujo visible

### `MainScene`

La escena principal adopta el acceso singleton a recursos y a la gestión de escenas, y registra `Logger` en `EventManager`. El comportamiento interactivo preserva la transición hacia `GameInScene`, el cierre por `Escape`, el cambio de modo de ventana y la alternancia de visualización de cuerpos, pero ahora sobre la nueva infraestructura compartida.

### `GameInScene`

La escena de juego replica el cambio de acceso a recursos y escenas mediante singletons y mantiene la generación dispersa de tiles ya presente en la iteración anterior. El ajuste principal de esta etapa se concentra en la adaptación de dependencias y cabeceras al nuevo esquema estructural.

## Dependencias externas visibles

- **Allegro 5** continúa siendo la biblioteca activa para rendering, ventana, eventos, temporizador, imágenes, audio, fuentes e input.
- **`nlohmann/json`** continúa presente en `include/` sin evidencia visible de integración en los archivos revisados de esta iteración.

## Resumen técnico de la versión

La revisión efectiva al **2 de noviembre de 2024** queda delimitada por estos movimientos:

- la interfaz pública del proyecto migra de forma amplia desde archivos `.h` hacia `.hpp`;
- `SceneManager` sustituye el uso directo de `GameSceneManager` y pasa a exponerse como singleton;
- `ResourceManager` pasa a resolverse como singleton y deja de ser liberado manualmente por `GameManager`;
- se incorpora `EventManager` como registro central de observadores y `GameEvent` como interfaz base;
- `MainScene` registra `Logger` y `LinkObject` comienza a emitir eventos visibles del runtime;
- la grilla principal de cuerpos incrementa su cobertura a `10 x 18` celdas;
- la evaluación de colisiones deja de invocar el callback inverso sobre el segundo objeto del par;
- el punto de entrada y las escenas se alinean con la nueva convención de cabeceras y managers.

La etapa deja al proyecto con una base más cercana a un runtime compartido por servicios globales, aunque todavía en un estado transicional en el que conviven archivos eliminados del esquema anterior y nuevas incorporaciones pendientes de consolidación en el historial.