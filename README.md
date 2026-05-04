# WandAllegroEngine

## Fecha efectiva

**31 de octubre de 2024**

## Descripción general

Esta versión documenta únicamente los cambios técnicos incorporados sobre el estado previamente descrito en el README committeado anterior. El enfoque de esta iteración no está en redefinir el framework base, sino en registrar qué partes se agregan, cuáles cambian de comportamiento y qué responsabilidades se reorganizan dentro del código visible.

## Objetivo técnico

El objetivo de esta actualización es consolidar mejor el ciclo de vida interno del framework y hacer más explícita la relación entre escenas, objetos, recursos y colisiones. En términos prácticos, esta versión introduce un control más claro sobre el reemplazo de escenas, mueve el registro de objetos hacia las escenas concretas, incorpora configuración de color de fondo por escena y simplifica la estructura interna del sistema de colisiones.

## Estructura del proyecto

La estructura general del proyecto se mantiene, pero esta iteración introduce cambios visibles dentro de estos puntos:

- `src/Scenes/`, donde cambia la forma de construir y reemplazar escenas activas.
- `src/Entities/`, donde las entidades dejan de registrarse a sí mismas en el manager global de objetos.
- `src/WandAllegroEngine/Managers/`, donde se amplía el control de cierre, reemplazo de escenas, color de fondo y ordenamiento de objetos.
- `src/WandAllegroEngine/Collisions/`, donde cambia la representación interna de la grilla de colisiones.
- `src/Utilities/Paths.h`, donde se agrega una nueva ruta de fondo.

## Arquitectura o sistemas principales

### Cambios en `GameManager`

`GameManager` incorpora ajustes funcionales concretos:

- se agrega `BackBufferColor` como estado persistente del manager;
- se agrega `SetBackBufferColor(ALLEGRO_COLOR color)` para que cada escena defina su propio color de limpieza;
- el loop principal deja de usar un color fijo hardcodeado y pasa a limpiar con el color almacenado;
- ante `ALLEGRO_EVENT_DISPLAY_CLOSE`, el cierre ya no depende de sacar una sola escena con `PopScene()`, sino de limpiar toda la pila mediante `gameSceneManager->Clear()`;
- se separa la lógica de liberación final en `End()`, que luego es invocada desde el destructor.

Estos cambios hacen más explícito el control del ciclo de vida del runtime y evitan que el cierre dependa únicamente del comportamiento de la escena activa.

### Cambios en el sistema de escenas

El sistema de escenas cambia en dos frentes principales.

En `GameScene`:

- el constructor pasa a recibir un nombre de escena;
- se agrega el atributo `Name`;
- se agrega `GetName()`;
- la operación de limpieza visible pasa de `Clean()` a `Clear()`.

En `GameSceneManager`:

- se agrega `ReplaceScene(GameScene* scene)`;
- se agrega `Clear()` como operación explícita para vaciar la pila completa;
- los puntos donde antes se llamaba `Clean()` pasan a usar `Clear()`;
- `MainScene` y `GameInScene` dejan de navegar con `PushScene()` para intercambio principal y pasan a usar `ReplaceScene()`.

Este cambio modifica el modelo de transición entre escenas: la iteración ya no trata el paso entre menú y juego como simple apilamiento, sino como reemplazo directo del estado activo.

### Cambios en el sistema de objetos

La modificación más importante del sistema de objetos está en la responsabilidad de registro.

En esta versión:

- `BackgroundObject`, `LinkObject` y `MapTileObject` dejan de llamar a `Game->gameObjectManager->Add(this)` dentro de sus constructores;
- la escena que crea cada entidad pasa a ser responsable de registrarla explícitamente en `GameObjectManager`.

Además:

- `GameObjectManager` incorpora `SortByLayer()` como método propio reutilizable;
- `GameObject::SetLayerId()` pasa a invocar `Game->gameObjectManager->SortByLayer()` cuando cambia la capa;
- se prohíbe la construcción por defecto de `GameObject` y `GameBodyObject` mediante constructores `= delete`.

La consecuencia técnica es clara: la construcción del objeto deja de implicar registro automático, y la propiedad del flujo de alta queda del lado de la escena que decide usarlo.

### Cambios en el sistema de colisiones

El sistema de colisiones presenta una reorganización interna importante.

En `GameBodyManager`:

- la grilla deja de ser un único puntero dinámico y pasa a almacenarse como `std::vector<GameGridCollisions>`;
- la configuración visible cambia a una grilla activa de `9 x 12` con celdas de `100` unidades;
- durante cada `Update()`, los cuerpos se redistribuyen en la grilla activa y luego la grilla se limpia al final del ciclo;
- `Add()` deja de insertar directamente el cuerpo en la grilla en el momento de registro;
- `Clear()` limpia primero las grillas y luego destruye los cuerpos.

En `GameGridCollisions`:

- la estructura `Cell** Cells` se reemplaza por `std::vector<std::vector<Cell>>`;
- se elimina la gestión manual de memoria basada en `new[]` y `delete[]`;
- se agrega `GetCellByPosition(Vec2 position)`;
- se agrega `ClearGrid()`;
- se elimina `UpdatePositions()` del diseño visible;
- `AddObject()` cambia de comportamiento: si un cuerpo cae fuera de la grilla, destruye al objeto padre en lugar de ignorarlo silenciosamente.

El cambio de fondo es que la ocupación espacial ya no se actualiza de forma incremental, sino que se reconstruye en cada frame a partir de la posición actual de cada cuerpo.

### Cambios en el sistema de recursos y paths

La API de `ResourceManager` no cambia de forma estructural, pero sí cambia el uso visible desde las escenas.

- `MainScene` ahora carga dos fondos distintos, `Background1` y `Background2`.
- `src/Utilities/Paths.h` agrega `BG_SPACE_IMAGE_PATH`.
- cada escena define ahora de forma más explícita qué recursos carga y qué color de fondo establece.

## Dependencias externas visibles

En esta iteración no aparecen nuevas dependencias externas respecto al estado anterior.

- **Allegro 5** sigue siendo la dependencia activa del framework.
- **`nlohmann/json`** sigue presente vendorizado en `include/`, sin integración visible nueva en el código revisado de esta versión.

## Resumen técnico de la versión

Los cambios incorporados en esta fecha se concentran en estos puntos:

- se agrega reemplazo explícito de escenas mediante `ReplaceScene()`;
- se agrega limpieza global de escenas mediante `Clear()`;
- se renombra y reorganiza la limpieza de `GameScene` hacia `Clear()`;
- se traslada el registro de objetos desde los constructores hacia las escenas concretas;
- se agrega control de color de fondo por escena en `GameManager`;
- se agrega `CustomEnemy` como nueva entidad visible en `MainScene`;
- `GameInScene` reduce la generación visible de tiles de `50 x 30` a `20 x 20`;
- el sistema de colisiones cambia a contenedores estándar y reconstrucción espacial por frame;
- se agrega una segunda ruta de fondo en `Paths.h`.

Como documento de esta etapa, este README no reemplaza la explicación base del framework ya registrada anteriormente. Su función es dejar constancia precisa de lo que cambia en la versión efectiva al **31 de octubre de 2024**.
