# WandAllegroEngine

## Fecha efectiva

**3 de noviembre de 2024**

## Descripción general

La etapa efectiva al 3 de noviembre profundiza la reestructuración iniciada en la iteración anterior y extiende el esquema singleton a la administración de objetos y cuerpos. Con ello, `GameManager` deja de conservar managers de gameplay como estado propio y queda más concentrado en el ciclo principal, la ventana y el procesamiento de eventos de Allegro.

La revisión también desplaza la selección de la escena inicial fuera de `Update()`, refuerza la limpieza de servicios compartidos al cerrar escenas y completa la liberación explícita del registro de eventos.

## Objetivo técnico

Esta revisión queda centrada en tres frentes:

- unificar los managers principales del runtime bajo acceso global controlado;
- extraer del loop principal la responsabilidad de decidir la escena inicial;
- alinear la limpieza de escenas, objetos, cuerpos y eventos con el nuevo ciclo de vida compartido.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Managers/`, donde `GameBodyManager` y `GameObjectManager` son retirados y sustituidos por `BodyManager` y `ObjectManager`.
- `src/WandAllegroEngine/Models/`, donde `GameObject` y `GameScene` pasan a depender del nuevo esquema de managers globales.
- `src/Scenes/` y `src/Entities/`, donde las altas de objetos y cuerpos dejan de resolverse a través de miembros de `GameManager`.
- `src/main.cpp`, donde el arranque define de forma explícita la primera escena antes de entrar al loop principal.

## Arquitectura o sistemas principales

### Managers de objetos y cuerpos

La modificación dominante de esta etapa consiste en que `ObjectManager` y `BodyManager` reemplazan a `GameObjectManager` y `GameBodyManager` no solo en nombre, sino también en modo de acceso:

- ambos managers pasan a exponer `GetInstance()`;
- `GameManager` elimina sus punteros a managers de objetos y cuerpos;
- escenas, entidades y modelos comienzan a operar directamente sobre `WandEngine::ObjectManager::GetInstance()` y `WandEngine::BodyManager::GetInstance()`.

El efecto técnico es una homogeneización del runtime: escenas, recursos, eventos, objetos y cuerpos ya no se distribuyen entre servicios globales y servicios retenidos por `GameManager`, sino que convergen en un mismo patrón de acceso compartido.

### Arranque y control de la escena inicial

`GameManager::Update()` deja de crear internamente `MainScene`. En su lugar:

- `main.cpp` incluye ahora la escena inicial de manera explícita;
- `GameManager` incorpora `SetFirstScene(GameScene* scene)`;
- `SceneManager` agrega `Empty()` para permitir que la primera carga se haga solo cuando la pila aún no contiene escenas.

Con este cambio, la preparación del estado inicial deja de estar incrustada en el loop principal y queda definida antes de iniciar la ejecución continua del juego.

### Limpieza de runtime compartido

La revisión refuerza la liberación de memoria y la limpieza de estado en servicios ya compartidos:

- `EventManager` incorpora `Clear()` y destructor propio para destruir observadores registrados;
- `GameScene::Clear()` vacía explícitamente `BodyManager`, `ObjectManager` y `ResourceManager`;
- los destructores de `ObjectManager` y `BodyManager` consolidan la liberación de entidades y cuerpos desde su propio ámbito.

Este ajuste reduce la dependencia de liberaciones dispersas y hace más coherente el cierre de una escena con el vaciado del estado global asociado a ella.

## Escenas y flujo visible

### `MainScene`

La escena principal conserva el comportamiento jugable inmediato de la iteración anterior, pero queda adaptada por completo al uso de `ObjectManager` como servicio global para registrar fondo, jugador y enemigo. El alta de `Logger` se mantiene en `EventManager`, ahora con limpieza explícita del contenedor al finalizar el ciclo de vida del manager.

### `GameInScene`

La escena de juego replica el cambio de acceso a `ObjectManager` para poblar jugador y tiles. La lógica visible de poblamiento aleatorio no se altera en esta etapa; el cambio se concentra en el desplazamiento de dependencias hacia los managers globales.

## Dependencias externas visibles

- **Allegro 5** continúa siendo la biblioteca activa para rendering, ventana, eventos, temporizador, imágenes, audio, fuentes e input.
- **`nlohmann/json`** continúa presente en `include/` sin evidencia visible de integración en los archivos revisados de esta iteración.

## Resumen técnico de la versión

La revisión efectiva al **3 de noviembre de 2024** queda delimitada por estos movimientos:

- `BodyManager` y `ObjectManager` sustituyen a `GameBodyManager` y `GameObjectManager` como servicios singleton del runtime;
- `GameManager` elimina los punteros propios a managers de objetos y cuerpos;
- `main.cpp` pasa a fijar la escena inicial mediante `SetFirstScene(new MainScene(Game))` antes de llamar `Update()`;
- `SceneManager` incorpora `Empty()` para controlar la carga inicial de la pila de escenas;
- `GameScene::Clear()` vacía cuerpos, objetos y recursos desde el nuevo esquema compartido;
- `EventManager` agrega limpieza explícita y destructor para liberar observadores registrados;
- escenas y entidades sustituyen el acceso a managers retenidos por `GameManager` por llamadas directas a `ObjectManager` y `BodyManager`.

La etapa deja al proyecto más cerca de una arquitectura en la que `GameManager` coordina el loop y la infraestructura de ventana, mientras que el estado jugable y sus servicios auxiliares quedan concentrados en managers globales con ciclo de vida propio.