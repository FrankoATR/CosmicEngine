# WandAllegroEngine

## Fecha efectiva

**4 de noviembre de 2024**

## Descripción general

La etapa efectiva al 4 de noviembre avanza sobre la centralización del runtime y desplaza tanto la entrada como el ciclo de dibujo y actualización de escena hacia servicios específicos del framework. A partir de esta revisión, teclado, mouse y joystick pasan a procesarse mediante `InputManager`, mientras que la presentación y el ciclo activo de escena se concentran en `SceneManager`.

En paralelo, `GameManager` adopta un papel más acotado como coordinador de infraestructura, y `GameScene` incorpora un modelo explícito de carga asíncrona con hilo propio, progreso sincronizado y tareas diferidas para el hilo principal. El resultado visible es una reducción adicional del acoplamiento entre loop principal, escenas y subsistemas del motor.

## Objetivo técnico

Esta revisión queda centrada en tres frentes:

- separar la captura de entrada, la actualización de escena y el dibujo del loop principal básico;
- eliminar dependencias directas a una instancia inyectada de `GameManager` en escenas y entidades;
- trasladar la carga de escenas a un flujo asíncrono controlado por el propio framework.

## Estructura del proyecto

La diferencia visible respecto a la revisión anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Managers/`, donde `InputManager` se incorpora como servicio de entrada, `GameManager` se reduce al ciclo base del runtime y `SceneManager` asume actualización, dibujo y color de fondo.
- `src/WandAllegroEngine/Models/`, donde `GameScene` incorpora primitives de carga asíncrona y `GameObject` abandona la necesidad de conservar una referencia directa a `GameManager`.
- `src/Scenes/` y `src/Entities/`, donde la capa de uso migra al nuevo acceso global para input y escena activa.
- `src/main.cpp`, donde el arranque deja de crear manualmente el manager del juego y pasa a operar sobre `WandEngine::GameManager::GetInstance()`.

## Arquitectura o sistemas principales

### Entrada del usuario

`InputManager` se incorpora como subsistema dedicado para la captura y consulta de entrada. Su implementación visible introduce:

- inicialización y registro de teclado, mouse y joystick sobre la cola de eventos del motor;
- procesamiento centralizado de eventos mediante `ProcessEvent(const ALLEGRO_EVENT&)`;
- separación de estados `KeyDown`, `KeyUp` y `KeyRelease` para teclado y mouse;
- reinicio por frame de eventos transitorios desde `Update()`.

Con este ajuste, escenas y entidades dejan de depender del mapa `keyState` y del análisis directo de `ALLEGRO_EVENT` para responder al input.

### `GameManager` como coordinador mínimo

La revisión convierte `GameManager` en singleton y reduce su superficie a responsabilidades de infraestructura y loop base:

- `main.cpp` deja de construir `new GameManager(...)` y usa `WandEngine::GameManager::GetInstance()`;
- el tamaño de pantalla se establece a través de `SetWindows_Size()` después de `Init()`;
- el loop principal invoca `InputManager::Update()`, consume la cola con `al_get_next_event()` y delega el trabajo de escena a `SceneManager`;
- desaparecen de su interfaz visible el control directo de color de fondo, el mapa `keyState` y el dibujo o actualización directa de objetos y cuerpos;
- la limpieza final se concentra en `Clear()` y ya no depende de destrucción manual del objeto principal.

Con ello, `GameManager` deja de ser el punto que decide y ejecuta toda la lógica visual de runtime, y pasa a coordinar la infraestructura sobre la que operan los demás managers.

### `SceneManager` como controlador del ciclo de escena

`SceneManager` amplía su rol y pasa a controlar el ciclo operativo de la escena activa:

- inicia la carga de cada escena mediante `StartSceneLoading()`;
- ejecuta `Update()` solo cuando la carga ha finalizado;
- ejecuta `UpdateLoadingScene()` y `DrawLoadingScene()` mientras la escena permanece en fase de carga;
- asume el dibujo general del frame a través de `Draw()`;
- incorpora `SetBackBufferColor()` y mantiene el color de limpieza fuera de `GameManager`.

Este ajuste desplaza el corazón del flujo de escena desde el loop principal hacia un manager especializado que ya no solo administra la pila, sino también su tránsito entre carga y ejecución.

### Carga asíncrona de escenas

`GameScene` incorpora una base explícita para carga concurrente:

- cada escena dispone ahora de `std::thread loadingThread`;
- el progreso de carga queda protegido por `std::mutex` y se consulta mediante `IsProgressLoadingSceneComplete()`;
- aparecen `AddMainThreadTask()` y `ExecuteMainThreadTasks()` para diferir trabajo que debe ejecutarse en el hilo principal;
- `StartLoadingThread()`, `IsLoadingThreadJoinable()` y `JoinLoadingThread()` formalizan el ciclo de vida del hilo de carga;
- `DrawLoadingScene()` se agrega como punto dedicado para representar el estado visual de carga.

El framework pasa así de una carga implícita y secuencial a un esquema preparado para inicialización concurrente con coordinación explícita entre hilo de carga e hilo principal.

### Desacoplamiento de escenas y objetos

`GameScene` y `GameObject` eliminan la dependencia constructiva respecto a `GameManager`:

- `GameScene` pasa a construirse solo con el nombre de la escena;
- las implementaciones concretas de escena dejan de recibir `GameManager*` en sus constructores;
- `GameObject` y las entidades derivadas dejan de almacenar o recibir el puntero `Game`;
- operaciones antes resueltas mediante esa referencia pasan a redistribuirse entre managers globales especializados según su responsabilidad.

La consecuencia directa es que escenas y entidades quedan ligadas al runtime por interfaz global compartida, no por inyección explícita del manager principal.

## Integración del framework

Las implementaciones visibles del framework ya utilizan el nuevo modelo en tres frentes:

- el control interactivo se resuelve mediante consultas directas a `InputManager` en lugar de inspeccionar eventos crudos o estados locales replicados;
- el ciclo de actualización y dibujo queda canalizado por `SceneManager` en lugar de ejecutarse de forma dispersa desde el loop principal;
- la carga de escena dispone de primitives explícitas para progreso, dibujo de carga y ejecución diferida en el hilo principal.

Con ello, la capa de uso del motor pasa a consumir los servicios del framework desde interfaces unificadas y deja de depender de referencias manuales al manager principal o de un loop central sobrecargado de responsabilidades.

## Dependencias externas visibles

- **Allegro 5** continúa siendo la biblioteca activa para rendering, ventana, eventos, temporizador, imágenes, audio, fuentes e input.
- **`nlohmann/json`** continúa presente en `include/` sin evidencia visible de integración en los archivos revisados de esta iteración.

## Resumen técnico de la versión

La revisión efectiva al **4 noviembre de 2024** queda delimitada por estos movimientos:

- se incorpora `InputManager` como servicio singleton para teclado, mouse y joystick;
- `GameManager` pasa a exponerse mediante `GetInstance()` y deja de instanciarse manualmente desde `main.cpp`;
- el loop principal procesa entrada mediante `InputManager::ProcessEvent()` y delega el ciclo de escena en `SceneManager`;
- `SceneManager` asume actualización, dibujo, color de fondo y transición entre carga y ejecución de escenas;
- `GameScene` incorpora hilo de carga, progreso sincronizado, cola de tareas para el hilo principal y dibujo específico de pantalla de carga;
- `GameScene`, `GameObject` y sus derivados eliminan la dependencia constructiva respecto a `GameManager*`;
- las implementaciones construidas sobre el motor migran su control interactivo a consultas directas sobre `InputManager`;
- responsabilidades antes concentradas en `GameManager` se redistribuyen entre managers globales especializados.

La etapa deja al proyecto más orientado a un runtime gobernado por servicios globales especializados, con menor dependencia entre escenas, entidades y la instancia principal del juego, y con una base más clara para carga asíncrona de escenas dentro del propio framework.