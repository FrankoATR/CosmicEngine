# WandAllegroEngine

## Fecha efectiva

**24 de septiembre de 2024**

## Descripcion general

`WandAllegroEngine` constituye la base inicial del proyecto antes de la evolucion hacia la estructura moderna del framework. En este estado, el motor se presenta como un framework 2D orientado a juegos construidos sobre Allegro 5, con una organizacion centrada en managers, escenas, objetos, recursos y colisiones.

La responsabilidad principal del framework es inicializar la plataforma, abrir la ventana del juego, mantener el ciclo principal, actualizar las escenas activas, actualizar los objetos registrados, procesar entradas de teclado y raton mediante Allegro, dibujar el contenido visible y administrar recursos graficos basicos como bitmaps, spritesheets y fuentes.

Aunque el repositorio ya incluye `nlohmann` dentro de `include/`, el nucleo funcional visible de esta version se concentra principalmente en Allegro 5 y en una arquitectura propia de clases base y managers.

## Objetivo tecnico del framework

En esta etapa, el framework busca resolver una base comun para juegos 2D con las siguientes capacidades:

- inicializacion centralizada del runtime y de los addons de Allegro;
- control del ciclo principal de juego con timer a 60 FPS;
- manejo de escenas cargadas en forma de pila;
- registro y actualizacion de objetos del juego;
- administracion de cuerpos de colision separados de la logica visual;
- carga y consulta de recursos visuales y tipograficos;
- soporte para pantalla completa, modo ventana y redimension de la ventana;
- propagacion del `deltaTime` para movimiento y logica por frame;
- soporte base para overlays de depuracion de colisiones.

## Estructura del proyecto

La estructura visible de esta version queda organizada de esta forma:

```text
src/
  main.cpp
  Scenes/
  Entities/
  Utilities/
  WandAllegroEngine/
    Managers/
    Models/
    Collisions/
include/
  allegro5/
  nlohmann/
assets/
```

### `src/main.cpp`

El punto de entrada crea una instancia de `GameManager` usando el tamano de la pantalla del sistema operativo y delega todo el arranque del juego al framework:

- construye el manager principal con un `Size(ScreenWidth, ScreenHeight)`;
- llama `Init()` para inicializar Allegro y los subsistemas internos;
- llama `Update()` para entrar al loop principal;
- destruye el manager al finalizar.

Esto deja claro que el framework se plantea para que la aplicacion cliente dependa de una unica clase coordinadora.

## Arquitectura principal

## `GameManager`

`GameManager` pasa a ser el centro de control del framework. Desde esta clase se coordinan:

- la ventana (`ALLEGRO_DISPLAY`);
- la cola global de eventos (`ALLEGRO_EVENT_QUEUE`);
- el timer principal (`ALLEGRO_TIMER`);
- el estado de teclado mediante `std::map<int, bool>`;
- el calculo de `deltaTime`;
- la creacion y destruccion de los managers internos.

Durante `Init()` el framework realiza lo siguiente:

- inicializa Allegro;
- activa audio, codecs, fuentes, TTF, primitivas, imagenes, raton y teclado;
- reserva un sample de audio;
- configura la ventana con OpenGL;
- crea la ventana y la pone en `fullscreen window`;
- crea la cola de eventos;
- registra fuentes de eventos de timer, mouse, teclado y display;
- crea un timer a `1.0 / 60`;
- instancia `ResourceManager`, `GameObjectManager`, `GameSceneManager` y `GameBodyManager`.

Durante `Update()` el manager:

- inserta la escena inicial `MainScene`;
- espera eventos con `al_wait_for_event`;
- detecta cierre de ventana;
- marca redraw en eventos del timer principal;
- actualiza escenas, objetos y cuerpos en cada iteracion;
- limpia la pantalla y dibuja objetos cuando la escena esta cargada;
- puede dibujar cuerpos de colision si esta activa la bandera de depuracion.

## Sistema de escenas

El framework queda apoyado en dos capas para escenas:

- `GameScene`, como clase base abstracta;
- `GameSceneManager`, como administrador de la pila de escenas.

### `GameScene`

`GameScene` define la interfaz minima que debe implementar una escena:

- `Init()` para carga e inicializacion;
- `Update(double deltaTime)` para logica por frame;
- `UpdateLoadingScene()` para una pantalla de carga basica;
- control de progreso mediante `SetProgressLoadingScene()` y consultas de estado.

Tambien mantiene una referencia al `GameManager`, lo cual permite a cada escena acceder directamente al estado global del engine, eventos, recursos y managers.

### `GameSceneManager`

`GameSceneManager` gestiona las escenas mediante una pila (`sceneStack`). Esto permite:

- hacer `PushScene()` para entrar a una nueva escena;
- hacer `PopScene()` para salir de la escena actual;
- mantener el juego corriendo mientras hubiera escenas activas;
- diferenciar entre una escena cargada y una escena en transicion.

En la practica, esta arquitectura permite flujos simples como:

- menu principal -> escena de juego;
- escena de juego -> volver al menu;
- salida del juego al vaciar la pila.

## Sistema de objetos

La base de objetos del framework queda construida alrededor de `GameObject` y `GameObjectManager`.

### `GameObject`

`GameObject` representa la entidad visual y logica elemental del juego. Cada objeto tiene:

- nombre (`ObjectName`);
- tipo (`DynamicEntity` o `StaticEntity`);
- identificador numerico;
- sprite de Allegro (`ALLEGRO_BITMAP*`);
- posicion actual y ultima posicion;
- tamano;
- capa de dibujado (`LayerId`);
- bandera de vida para ser removido del manager.

La clase expone comportamiento base para:

- `Draw()` usando `al_draw_tinted_scaled_rotated_bitmap`;
- `Update(float deltaTime)`;
- `OnCollision(GameObject* other)`;
- mover y redimensionar el objeto;
- restaurar la ultima posicion;
- destruir el objeto logicamente con `Destroy()`.

### `GameObjectManager`

`GameObjectManager` mantiene un vector de actores y asigna IDs incrementales. Sus responsabilidades son:

- registrar objetos con `Add()`;
- ordenar los objetos por `LayerId` despues de agregarlos;
- actualizar todos los objetos vivos;
- remover y destruir objetos marcados como no vivos;
- dibujar todos los objetos registrados;
- limpiar completamente la lista al cerrar.

Esto muestra que el framework ya establece una separacion clara entre la definicion del objeto y su administracion centralizada.

## Sistema de colisiones

La parte de colisiones queda dividida entre:

- `GameBodyObject`, como representacion fisica o de hitbox;
- `GameBodyManager`, como coordinador de los cuerpos;
- `GameGridCollisions`, como acelerador espacial por celdas.

### `GameBodyObject`

Cada cuerpo de colision almacena:

- tipo de cuerpo;
- identificador;
- posicion y tamano;
- referencia al `GameObject` padre.

El cuerpo no es el objeto jugable en si, sino su representacion para deteccion fisica. Esta separacion es importante porque ya muestra una intencion de desacoplar logica visual de logica de colision.

### `GameBodyManager`

`GameBodyManager` sincroniza cada cuerpo con la posicion de su objeto padre, elimina cuerpos de objetos destruidos y ejecuta el flujo de colisiones en cada update.

En esta version, el manager construye un `GameGridCollisions(Vec2(0, 0), 11, 19, 100)`, lo que implica:

- una grilla fija de colisiones;
- 11 filas por 19 columnas;
- celdas de 100 unidades;
- broad-phase basado en particion espacial discreta.

Tambien incorpora una funcion `RectToRectCollisionBody()` para chequeo AABB entre dos cuerpos rectangulares.

### `GameGridCollisions`

La estructura de grid administra celdas que contienen listas de objetos, y provee operaciones para:

- agregar y remover cuerpos de la grilla;
- actualizar posiciones de ocupacion;
- dibujar la grilla para depuracion;
- buscar colisiones entre celdas;
- resolver colisiones rectangulo-rectangulo.

Para esta etapa del framework, la grilla se convierte en el mecanismo principal de organizacion espacial de colisiones.

## Sistema de recursos

`ResourceManager` es el modulo dedicado a cargar y cachear recursos basicos. El manager mantiene mapas de:

- bitmaps individuales;
- spritesheets, almacenando bitmap y metadata de filas y columnas;
- fuentes.

Las operaciones visibles son:

- `loadBitmap(key, path)`;
- `loadSpriteSheet(key, path, files, columns)`;
- `loadFont(key, path, size)`;
- `getBitmap(key)`;
- `getBitmapRegionFromSpriteSheet(key, file, column)`;
- `getFont(key)`;
- `clear()`.

Con esto, el framework ya soporta una capa de recursos reutilizable sin obligar a cada escena u objeto a recargar archivos directamente.

## Manejo de entrada y ventana

El estado de entrada aun no queda encapsulado en un manager independiente. En su lugar:

- `GameManager` mantiene el ultimo `ALLEGRO_EVENT` global;
- cada escena inspecciona `Game->Event.type`;
- `MainScene` y `GameInScene` actualizan manualmente `Game->keyState` en eventos `KEY_DOWN` y `KEY_UP`.

Ese esquema permite una entrada funcional, aunque con fuerte acoplamiento entre escena y estado global del manager.

Ademas, el framework ya permite:

- alternar visualizacion de cuerpos con `ToggleShowBody()`;
- cambiar a pantalla completa con `SetWindows_FullScreenMode()`;
- volver a modo ventana con `SetWindows_WindowsMode()`;
- redimensionar la ventana con `SetWindows_Size()`;
- consultar si la ventana estaba en fullscreen.

## Flujo real visible en las escenas incluidas

Las escenas `MainScene` y `GameInScene` muestran como se usa el framework:

- cargan recursos desde rutas fijas en `Utilities/Paths.h`;
- inicializan el estado de teclas;
- construyen entidades como `LinkObject` y tiles del mapa;
- actualizan transiciones entre escenas con `PushScene()` y `PopScene()`;
- cambian opciones de ventana mediante combinaciones de teclado;
- actualizan la barra de progreso de carga con `SetProgressLoadingScene()`.

`GameInScene` tambien evidencia un uso orientado a mapas 2D basados en mosaicos, creando una cuadricula de tiles en tiempo de carga.

## Modelo de entidades derivadas

Las entidades de `src/Entities` funcionan como especializaciones de `GameObject`.

Por ejemplo, `LinkObject`:

- se registra a si mismo en `GameObjectManager`;
- crea automaticamente un `GameBodyObject` asociado y lo registra en `GameBodyManager`;
- responde a WASD para movimiento;
- define comportamiento en `OnCollision()`;
- puede usar una fuente Allegro para overlays de texto.

Este patron indica que el framework ya favorece la herencia para crear actores concretos del juego, dejando al manager la administracion global de vida, dibujo y colision.

## Estado tecnico del framework en esta version

En terminos de madurez, esta instantanea muestra un framework funcional pero todavia temprano. Sus rasgos tecnicos mas importantes son:

- arquitectura simple y centralizada alrededor de `GameManager`;
- fuerte acoplamiento entre escenas, eventos globales y managers;
- especializacion clara para juegos 2D;
- dependencia directa del ecosistema Allegro para render, input, fuentes y audio;
- managers separados para escenas, objetos, cuerpos y recursos;
- soporte de colision rectangular con optimizacion por grid;
- sistema de capas de dibujo y destruccion controlada de objetos.

Todavia no se observa una separacion mas avanzada por modulos independientes, configuracion por proyecto, escenas desacopladas del bootstrap, ni sistemas modernos de render, iluminacion, networking o persistencia integrados como en etapas posteriores del repositorio.

## Dependencias externas visibles en esta etapa

Las dependencias externas que pueden identificarse directamente en esta version son:

- **Allegro 5**: biblioteca principal sobre la que se construye el framework. Se utiliza para ventana, eventos, timer, input, dibujo 2D, primitivas, imagenes, fuentes, TTF y audio.
- **nlohmann/json**: distribuida dentro de `include/nlohmann/` como dependencia disponible del repositorio. En esta instantanea no se observa un subsistema JSON protagonista dentro del nucleo principal leido, pero la libreria ya forma parte de los headers incluidos por el proyecto.

## Resumen

`WandAllegroEngine` representa la primera base estructurada del framework: un motor 2D construido sobre Allegro 5, con managers propios para escena, objetos, cuerpos y recursos, soporte de colisiones por grid, manejo de sprites y fuentes, y una arquitectura suficientemente modular para crear escenas y entidades derivadas, aunque todavia con un fuerte acoplamiento interno y una orientacion marcadamente prototipica.