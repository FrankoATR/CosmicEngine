# WandAllegroEngine

## Fecha efectiva

**3 de noviembre de 2024**

## Descripción general

La etapa efectiva al 3 de noviembre avanza sobre la centralización del runtime y desplaza el manejo de entrada hacia un servicio dedicado. A partir de esta revisión, teclado, mouse y joystick dejan de interpretarse desde estados locales mantenidos por las escenas o por `GameManager`, y pasan a procesarse mediante `InputManager` sobre la cola global de eventos.

En paralelo, `GameManager` adopta también un acceso singleton y las escenas, entidades y objetos dejan de construirse alrededor de un puntero explícito al manager principal. El resultado visible es una reducción de acoplamiento entre lógica de juego, administración de escenas y lectura de input.

## Objetivo técnico

Esta revisión queda centrada en tres frentes:

- separar la captura y consulta de entrada del resto del loop principal;
- eliminar dependencias directas a una instancia inyectada de `GameManager` en escenas y entidades;
- consolidar el acceso al estado global del runtime mediante managers singleton.

## Estructura del proyecto

La diferencia visible respecto a la revisión anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Managers/`, donde aparece `InputManager` y `GameManager` cambia de una instancia administrada externamente a un singleton del motor.
- `src/WandAllegroEngine/Models/`, donde `GameObject` y `GameScene` abandonan la necesidad de recibir o conservar una referencia directa a `GameManager`.
- `src/Scenes/` y `src/Entities/`, donde constructores y flujo de actualización migran al nuevo acceso global para input, ventana y escena activa.
- `src/main.cpp`, donde el arranque deja de crear manualmente el manager del juego y pasa a operar sobre `WandEngine::GameManager::GetInstance()`.

## Arquitectura o sistemas principales

### Entrada del usuario

`InputManager` se incorpora como subsistema dedicado para la captura y consulta de entrada. Su implementación visible introduce:

- inicialización y registro de teclado, mouse y joystick sobre la cola de eventos del motor;
- procesamiento centralizado de eventos mediante `ProcessEvent(const ALLEGRO_EVENT&)`;
- separación de estados `KeyDown`, `KeyUp` y `KeyRelease` para teclado y mouse;
- reinicio por frame de eventos transitorios desde `Update()`.

Con este ajuste, escenas y entidades dejan de depender del mapa `keyState` y del análisis directo de `ALLEGRO_EVENT` para responder al input.

### `GameManager` como servicio global

La revisión convierte `GameManager` en singleton y reduce aún más su interfaz pública a responsabilidades de infraestructura:

- `main.cpp` deja de construir `new GameManager(...)` y usa `WandEngine::GameManager::GetInstance()`;
- el tamaño de pantalla se establece a través de `SetWindows_Size()` después de `Init()`;
- el loop principal invoca `InputManager::Update()` y consume la cola con `al_get_next_event()`;
- la limpieza final se concentra en `Clear()` y ya no depende de destrucción manual del objeto principal.

Este cambio alinea al manager principal con el patrón ya adoptado por escenas, objetos, cuerpos, recursos y eventos.

### Desacoplamiento de escenas y objetos

`GameScene` y `GameObject` eliminan la dependencia constructiva respecto a `GameManager`:

- `GameScene` pasa a construirse solo con el nombre de la escena;
- `MainScene` y `GameInScene` dejan de recibir `GameManager*` en sus constructores;
- `GameObject` y las entidades derivadas dejan de almacenar o recibir el puntero `Game`;
- operaciones antes resueltas mediante esa referencia, como cambiar el color de fondo o alternar la visualización de cuerpos, se atienden ahora a través de `WandEngine::GameManager::GetInstance()`.

La consecuencia directa es que escenas y entidades quedan ligadas al runtime por interfaz global compartida, no por inyección explícita del manager principal.

## Escenas y flujo visible

### `MainScene`

La escena principal reemplaza la lectura directa de eventos por consultas a `InputManager` para `Escape`, `Space`, `H`, `P`, `O`, `U` e `I`. También adapta la creación de objetos al nuevo esquema sin puntero `Game` y cambia el nombre visible del enemigo creado en escena a `Emerson`.

### `GameInScene`

La escena de juego replica la migración completa hacia `InputManager` y hacia `GameManager::GetInstance()` para el color de fondo y la alternancia de depuración de cuerpos. La creación de jugador y tiles conserva la lógica de poblamiento disperso, pero ya no depende de un manager inyectado en el constructor.

### `LinkObject`

La entidad del jugador sustituye el uso de `keyState` por consultas de tipo `KeyRelease` y `KeyDown` en `InputManager`. Además, la acción asociada a `Q` restaura el tamaño a `64 x 64` y emite el evento `Reset Size`, mientras que la colisión ofensiva pasa a comprobar el nombre `Emerson` en lugar de `Enemy`.

## Dependencias externas visibles

- **Allegro 5** continúa siendo la biblioteca activa para rendering, ventana, eventos, temporizador, imágenes, audio, fuentes e input.
- **`nlohmann/json`** continúa presente en `include/` sin evidencia visible de integración en los archivos revisados de esta iteración.

## Resumen técnico de la versión

La revisión efectiva al **3 de noviembre de 2024** queda delimitada por estos movimientos:

- se incorpora `InputManager` como servicio singleton para teclado, mouse y joystick;
- `GameManager` pasa a exponerse mediante `GetInstance()` y deja de instanciarse manualmente desde `main.cpp`;
- el loop principal procesa entrada mediante `InputManager::ProcessEvent()` en lugar de delegar ese estado a escenas;
- `GameScene`, `GameObject` y sus derivados eliminan la dependencia constructiva respecto a `GameManager*`;
- `MainScene` y `GameInScene` migran su control interactivo a consultas directas sobre `InputManager`;
- operaciones de infraestructura, como el color de fondo, el cambio de modo de ventana y la visualización de cuerpos, pasan a invocarse desde `WandEngine::GameManager::GetInstance()`;
- `LinkObject` ajusta su comportamiento visible de input y su lógica de colisión para el enemigo nombrado `Emerson`.

La etapa deja al proyecto más orientado a un runtime gobernado por servicios globales especializados, con menor dependencia entre escenas, entidades y la instancia principal del juego.