# WandEngine

## Fecha efectiva

**11 de febrero de 2025**

## Descripción general

En esta etapa el cambio dominante deja de ser la migración de backend gráfico y pasa a ser la consolidación del motor nuevo como framework de trabajo. La base OpenGL ya no solo sirve para abrir ventana, renderizar y ejecutar el loop principal, sino que empieza a incorporar servicios internos para persistencia estructurada, administración más robusta del ciclo de vida de objetos y soporte de tooling dentro del propio runtime.

La evidencia visible de esta versión está en varios frentes que se cruzan entre sí: aparece un `DataBaseManager` apoyado en SQLite para guardar y reconstruir datos del mundo, `GameManager` añade ingestión de archivos soltados sobre la ventana, `GameObject` amplía su modelo base con aceleración, límites de velocidad, rotación angular, clonación operativa y referencias controladas entre objetos administrados por el motor, y además se afinan tanto la gestión de reproducción musical como la capa de importación y dibujo de modelos.

Con ello el proyecto deja de mostrar solo una reconstrucción técnica del engine y empieza a perfilar una capa de servicios internos más completa: gestionar objetos, persistir estado, recibir entradas externas y reutilizar esos mecanismos desde cualquier escena o arranque basado en el framework.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- convertir el engine OpenGL en una base utilizable para persistencia y tooling interno;
- enriquecer el modelo base de objetos con estado físico y semántica de referencias más útil para subsistemas del framework;
- consolidar servicios reutilizables de runtime para guardado, carga, ingestión de archivos y reinicio controlado de escenas.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `include/WandEngine/Managers/DataBase/`, donde aparece `DataBaseManager` como nueva pieza visible del framework para apertura, limpieza, consulta y persistencia de niveles apoyados en SQLite.
- `include/WandEngine/Models/GameObject.*`, donde el modelo base añade aceleración, velocidad angular, límites mínimo y máximo de velocidad, rotación en coma flotante y manejo explícito de referencias duplicadas mediante `pointer_copies`.
- `include/WandEngine/Managers/Object/ObjectManager.cpp`, donde la eliminación de objetos se alinea con ese nuevo sistema de referencias y con búsquedas más útiles para el editor, como selección por área o por posición del mouse.
- `include/WandEngine/Managers/GameManager.*`, donde se añade soporte visible para archivos soltados sobre la ventana y se abre una ruta de importación/carga desde el runtime.
- `include/WandEngine/Models/GameScene.*`, donde `Reset()` pasa a formar parte del contrato base de escena y se reorganiza el orden de actualización de managers.
- `include/WandEngine/Models/UIElements/UIButton.*` y `include/WandEngine/Managers/UI/UIManager.*`, donde se sigue delimitando una capa de UI separada y reutilizable dentro del runtime.
- `include/WandEngine/Managers/Audio/Music/MusicManager.*`, donde la reproducción musical gana seguimiento explícito de canales por clave, control de volumen por pista y reposicionamiento temporal dentro del stream.
- `include/WandEngine/Models/Mesh/*` y `include/WandEngine/Models/Model/*`, donde la capa de mallas y modelos queda más acotada a importación, materiales, buffers y dibujado directo.

## Arquitectura o sistemas principales

### Persistencia de niveles dentro del engine

La incorporación de `DataBaseManager` es el cambio más nuevo y más estructural de esta etapa. El engine ya no se limita a instanciar niveles desde arreglos hardcodeados dentro de la escena, sino que empieza a exponer una ruta interna para guardar y reconstruir contenido mediante SQLite.

`DataBaseManager` ya resuelve tareas visibles como:

- apertura diferenciada de base de datos para guardado y carga;
- limpieza completa de tablas previas antes de persistir un nivel nuevo;
- ejecución de SQL arbitrario y consultas con callback;
- creación dinámica de tablas;
- inserción de datos por entidad;
- cierre con `wal_checkpoint` y optimización final.

Sobre esa base, el framework ya deja preparado un contrato claro para que las entidades del mundo serialicen y reconstruyan su estado desde tablas persistidas. La persistencia ya no queda completamente fuera del runtime, sino que empieza a formar parte de la infraestructura visible del engine.

### Runtime con servicios reutilizables de tooling

El cambio importante de esta etapa es que varias capacidades que antes habrían vivido fuera del motor ahora aparecen como servicios internos reutilizables. Sin depender de una escena concreta, el framework ya deja visibles operaciones como:

- abrir una base de datos para guardado o para carga;
- limpiar el estado persistido anterior antes de reconstruir datos nuevos;
- ejecutar consultas con callback desde el propio runtime;
- invalidar referencias externas cuando un objeto administrado es destruido;
- recibir archivos soltados sobre la ventana para integrarlos al flujo del engine;
- reiniciar escenas mediante un hook base común en `GameScene`.

Eso cambia el papel del engine dentro del proyecto: ya no es solo un runtime que reproduce contenido definido en código, sino una base que empieza a incorporar infraestructura de tooling lista para ser consumida por cualquier capa superior.

### Audio con control más fino por recurso

La capa de audio también se vuelve más precisa en el nivel del framework. `MusicManager` no se limita a cargar y disparar reproducción, sino que pasa a conservar un mapa de `FMOD_CHANNEL*` por clave musical. Eso habilita operaciones más específicas dentro del runtime:

- detener una pista concreta sin afectar al resto;
- ajustar volumen por recurso ya reproducido;
- reposicionar la reproducción en milisegundos con `SetPosition()`;
- mantener una separación más clara entre el registro de sonidos cargados y los canales activos en ejecución.

No cambia la dependencia base, pero sí mejora la semántica del subsistema: la música deja de ser un disparo opaco y pasa a quedar bajo un control más administrable desde el engine.

### Modelo de objeto más útil para tooling y simulación

`GameObject` amplía su contrato de forma considerable. Además de posición, tamaño y color, el modelo base ahora expone:

- `Velocity`, `Acceleration`, `MaxVelocity` y `MinVelocity`;
- `Rotation` en `float` y `AngularVelocity`;
- `UpdateLastPosition()` separado del propio `UpdatePosition()`;
- clonación virtual y `Reset()` como hook reutilizable;
- un sistema explícito de referencias externas mediante `pointer_copies`, `makeReference()` y `unRerence()`.

La importancia de esto no está solo en la física. El engine gana una semántica de objeto más rica para dos problemas prácticos del framework:

- que distintos managers o capas superiores puedan mantener referencias vivas a objetos administrados sin desincronizarse cuando se destruyen;
- que el runtime pueda clonar, reinstanciar o limpiar entidades sin dejar punteros colgantes dentro del motor.

`ObjectManager` se adapta a ese contrato y, al eliminar un objeto, invalida las copias registradas antes de liberar memoria. Esa decisión mejora la robustez del motor justo en el punto donde editor, escena y managers empiezan a compartir referencias sobre las mismas entidades.

### Física base más expresiva en 2D

La semántica de simulación también se vuelve más coherente en el nivel del framework. `ObjectManager::Update()` ya separa el `Update()` específico de cada actor de la integración física base vía `UpdatePosition()`, y `GameObject` resuelve internamente la suma de aceleración, el clamp de velocidad y la rotación angular.

El resultado es una base más estable que la de versiones anteriores, porque el movimiento deja de depender de ajustes dispersos por implementación concreta y empieza a descansar más en el modelo base del motor.

### Runtime preparado para ingestión externa mínima

`GameManager` añade soporte de `DropCallback` y una cola interna de archivos soltados sobre la ventana. Con ello el runtime queda preparado para abrir recursos externos desde el propio motor y conectarlos a flujos de carga o importación sin depender de una herramienta aparte.

Aunque el flujo todavía es básico, el paso técnico importante es claro: el engine empieza a aceptar entradas externas del usuario como parte de su propia infraestructura de tooling.

### Capa 3D más contenida en torno a importación y draw

También hay una depuración visible en `Mesh` y `Model`. La infraestructura sigue apoyándose en `Assimp`, `stb_image` y OpenGL, pero el contrato queda más concentrado en lo que el framework efectivamente necesita en esta etapa:

- importar nodos, mallas y materiales desde archivo;
- convertir vértices, normales, UV y texturas a buffers utilizables por OpenGL;
- dibujar colecciones de mallas mediante `Shader` sin capas extra de complejidad todavía no consolidadas.

Ese ajuste no expande capacidades nuevas de cara al usuario final, pero sí limpia la frontera interna del engine: la capa 3D queda definida como infraestructura base de carga y render, en lugar de adelantarse a abstracciones que todavía no se usan de forma estable dentro del framework.

### UI y escena con hooks más reutilizables

La capa de escena también se ajusta a esta orientación de tooling. `GameScene` incorpora `Reset()` como operación virtual y reordena `UpdateManagers()` para ejecutar primero la lógica propia de la escena antes de delegar la actualización al resto de managers. En paralelo, `UIButton` se mantiene como elemento interactivo visible y `UIManager` sigue consolidándose como capa separada para controles de interfaz dentro del runtime.

Aunque todavía no hay una UI de edición completa, la base ya queda preparada para mover parte del editor desde atajos de teclado hacia controles visibles del motor.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- el runtime ya puede crear, actualizar, clonar, destruir, guardar y reconstruir entidades del mundo usando managers y modelos base del propio engine;
- la capa de arranque del motor ya dispone de hooks para recibir archivos externos, reiniciar escenas y delegar servicios de persistencia sin acoplar esas tareas a un framework externo.

Con ello dejé una base más cercana a una herramienta real de creación sobre el motor. La mejora principal de esta etapa no es tanto visual como arquitectónica: el engine deja de ser solo infraestructura de ejecución y empieza a asumir también responsabilidades de edición, persistencia y administración segura de objetos dentro del mismo flujo de trabajo.

## Dependencias externas visibles

No aparecen sustituciones grandes de dependencias respecto a la etapa del 8 de febrero, pero sí se vuelve mucho más visible el papel de `sqlite3` dentro del framework. En esta versión deja de estar solo presente como biblioteca enlazada y pasa a participar de forma directa en el flujo del engine mediante `DataBaseManager` y la persistencia de entidades del nivel.

En paralelo, `FMOD`, `Assimp` y `stb_image` quedan algo más integrados en la semántica interna del motor: `FMOD` con control de canales por pista, y `Assimp` más `stb_image` como base concreta de la capa `Model`/`Mesh` ya simplificada alrededor de importación y draw.

## Resumen técnico de la versión

La etapa efectiva al **11 de febrero de 2025** queda delimitada por estos movimientos:

- incorporé `DataBaseManager` y volví explícita la persistencia de niveles dentro del engine usando `sqlite3`;
- reforcé la capa de runtime con servicios reutilizables para guardado, carga, ingestión de archivos y reinicio de escenas;
- amplié `GameObject` con aceleración, velocidad angular, límites de velocidad, clonación y manejo controlado de referencias compartidas;
- adapté `ObjectManager` para invalidar referencias externas al destruir objetos y para sostener mejor selección por mouse o por área;
- incorporé soporte de archivos soltados sobre la ventana para cargar niveles persistidos desde el propio runtime;
- incorporé `Reset()` al contrato base de `GameScene` y seguí separando la UI como subsistema reutilizable dentro del framework.
- afiné `MusicManager` para controlar canales, volumen y posición de reproducción por recurso musical;
- simplifiqué la capa `Mesh`/`Model` para dejarla concentrada en importación, materiales, buffers y dibujado directo.

Con esta etapa dejé el motor en una versión donde el backend OpenGL ya no solo sirve para correr el juego de ejemplo: también empieza a sostener herramientas internas de edición, persistencia y manipulación estructurada del contenido.