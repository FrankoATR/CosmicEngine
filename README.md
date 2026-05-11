# WandAllegroEngine

## Fecha efectiva

**27 de diciembre de 2024**

## Descripción general

En esta etapa el cambio dominante pasa al ciclo de escena y al modelo de movimiento del runtime. La base del framework deja de concentrar toda la preparación dentro de `Init()` y empieza a separar de forma visible la carga de recursos, la pantalla de carga y la inicialización final de la escena, con un flujo más claro entre `LoadResources()`, el hilo de carga y la entrada efectiva al update principal.

Sobre esa reorganización del ciclo también amplié el modelo base de movimiento para trabajar con velocidad vectorial en lugar de una única magnitud escalar. Esa decisión ya se refleja en cámara, fondos, jugador y escena principal, donde el template empieza a comportarse menos como una demo fija y más como una base jugable con fullscreen adaptado a la resolución real, transición de carga y modos de movimiento más específicos.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- separar explícitamente la carga de recursos de la inicialización operativa de cada escena;
- refinar cámara, pantalla de carga y arranque de ventana para un flujo visual más consistente;
- extender el modelo base de movimiento hacia velocidades vectoriales y usarlo en modos de jugador y elementos del escenario.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `include/WandEngine/Models/GameScene.*`, donde el contrato visible de escena incorpora `LoadResources()` y reajusta el manejo de la pantalla de carga y del hilo de carga.
- `include/WandEngine/Managers/SceneManager.*`, donde el cambio de escena ya coordina de forma más explícita carga de recursos, activación del hilo de carga y reemplazo diferido de escenas.
- `include/WandEngine/Managers/CameraManager.*`, donde la cámara expone tamaño, punto de foco y reinicio más claros para soportar carga y seguimiento vertical dentro del juego.
- `include/WandEngine/Models/GameObject.*`, donde la velocidad pasa a representarse con `WAND_VEC2` y el desplazamiento deja de depender de un único escalar.
- `src/Scenes/MainScene.*`, donde la escena principal se reorganiza para cargar recursos por separado, controlar música por nivel, centrar la cámara durante carga y añadir desplazamiento vertical progresivo.
- `src/Entities/Player.*`, `Background.*`, `Ground.*` y `Spike.*`, donde el ejemplo adapta colisiones y movimiento al nuevo modelo vectorial y al soporte de modos de jugador.
- `src/main.cpp` y `include/WandEngine/Managers/GameManager.cpp`, donde el arranque visible del programa pasa a usar tamaño de pantalla del sistema y fullscreen por ventana como configuración efectiva.

## Arquitectura o sistemas principales

### Separación entre carga de recursos e inicialización

`GameScene` cambia su contrato visible y deja de asumir que toda la preparación ocurre dentro de `Init()`. A partir de esta etapa, cada escena expone `LoadResources()` como punto explícito para cargar bitmaps, fuentes, audio y demás dependencias previas a la ejecución efectiva.

`SceneManager` se alinea con ese cambio y ahora dispara la secuencia visible en este orden:

- creación o reemplazo de la escena;
- carga inicial de recursos mediante `LoadResources()`;
- inicio del hilo de carga con `StartLoadingThread()`;
- transición desde `DrawLoadingScene()` y `UpdateLoadingScene()` hasta el update operativo normal.

Con ello el framework deja una separación más clara entre preparar contenido y ejecutar la lógica viva de la escena.

### Pantalla de carga y cámara más controladas

La cámara pasa a participar explícitamente en el flujo de carga. `GameScene::UpdateLoadingScene()` y la implementación específica de `MainScene` recentran el foco durante la carga, evitando arrastres del estado anterior mientras la escena todavía no entra a su ciclo normal.

Además, `CameraManager` amplía su contrato visible con:

- lectura del tamaño de cámara;
- lectura del punto de foco efectivo;
- `Reset()` para recomponer el estado inicial según el tamaño de ventana.

Sobre esa base, `MainScene` introduce un seguimiento más estable del jugador, donde la cámara ya no se limita a seguir un punto fijo horizontal, sino que ajusta gradualmente su componente vertical con apoyo de un timer específico.

### Movimiento base con velocidad vectorial

`GameObject` abandona la velocidad escalar única y pasa a trabajar con `WAND_VEC2 Velocity`. Desde esta etapa, `UpdatePosition()` usa velocidad y dirección por eje, lo que deja una base más útil para objetos cuyo desplazamiento horizontal y vertical no comparten la misma magnitud ni la misma semántica.

Ese ajuste visible se aprovecha de inmediato en:

- `Player`, cuyo desplazamiento ya distingue mejor avance horizontal y respuesta vertical;
- `Background`, que mantiene desplazamiento lateral autónomo mientras el jugador sigue activo;
- futuros modos de movimiento que no encajan bien en una sola velocidad global.

### Modos de jugador y control de vuelo

`Player` deja de estar restringido al comportamiento base del cubo y empieza a modelar modos concretos mediante `PlayerMode`. En esta etapa ya queda visible un comportamiento diferenciado entre:

- `Normal`, con salto puntual, gravedad acumulativa y rotación por cuartos de vuelta;
- `Ship`, con empuje sostenido, caída progresiva, rotación acotada y resolución distinta respecto a los bloques.

Esa ampliación no solo afecta la lógica de input, sino también la forma en que se interpretan las colisiones superior e inferior contra `SolidBlock`.

### Escena principal más desacoplada y progresiva

`MainScene` redistribuye sus responsabilidades para alinearse con el nuevo flujo del framework. La escena ahora:

- carga recursos y audio desde `LoadResources()`;
- deja `Init()` como cierre liviano de la carga;
- fija música según el nivel activo sin duplicar esa selección en la lógica de update;
- crea timers específicos para respawn y movimiento vertical de cámara;
- usa una pantalla de carga centrada antes de entrar al juego.

Con ello la escena principal queda más cerca de un patrón reutilizable para niveles futuros que de una inicialización monolítica única.

### Arranque ajustado a la resolución real

`main.cpp` y `GameManager` terminan de orientar la ejecución a fullscreen por ventana usando el tamaño real de la pantalla del sistema en lugar de una resolución fija de desarrollo. Ese cambio reduce la dependencia de medidas hardcodeadas en el arranque inicial y alinea mejor cámara, ventana y escena desde el primer frame.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- la escena principal ya separa carga de recursos, transición de carga y ejecución de gameplay dentro del ciclo de escenas del engine;
- el jugador, la cámara y los fondos ya usan el nuevo modelo de velocidad vectorial y timers especializados para producir seguimiento, vuelo y desplazamiento más controlados.

Con ello dejé una base más útil para seguir construyendo escenas con carga explícita, transición visual y modos de movimiento más expresivos sin tener que reescribir el flujo principal del runtime.

## Dependencias externas visibles

No aparecen dependencias externas nuevas respecto a la etapa anterior. Se mantiene el mismo entorno visible basado en CMake, Allegro y las bibliotecas auxiliares ya declaradas por el proyecto.

## Resumen técnico de la versión

La etapa efectiva al **27 de diciembre de 2024** queda delimitada por estos movimientos:

- separé la carga de recursos del resto de la inicialización de escena mediante `LoadResources()` y ajusté `SceneManager` a ese nuevo ciclo;
- reforcé la pantalla de carga y el control de cámara para mantener un foco estable antes de entrar al gameplay;
- cambié `GameObject` a un modelo de velocidad vectorial con `WAND_VEC2 Velocity`;
- amplié `Player` con modos de movimiento diferenciados, incluyendo una lógica visible para `Ship` además del modo `Normal`;
- añadí timers específicos para movimiento vertical de cámara y reajusté la escena principal alrededor de ese seguimiento;
- adapté el arranque del programa a la resolución real de pantalla y al uso efectivo de fullscreen por ventana.

Con esta etapa dejé el framework más preparado para escenas que cargan contenido de forma explícita y para entidades cuyo comportamiento ya requiere movimiento, cámara y transición de estado más finos que en la versión anterior.