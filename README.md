# WandAllegroEngine

## Fecha efectiva

**31 de octubre de 2024**

## Descripción general

La etapa efectiva al 31 de octubre reorganiza la activación de objetos del framework alrededor de una fase explícita de inicialización posterior al registro. A partir de este ajuste, la creación de cuerpos de colisión deja de resolverse en los constructores concretos y pasa a ejecutarse cuando el objeto ya ha sido incorporado al `GameObjectManager` y dispone de un identificador válido.

El resultado inmediato es un flujo más coherente entre alta de entidades, asignación de IDs, construcción de cuerpos y sincronización con el sistema de colisiones. Sobre esa base también se ajustan el contenido de escenas concretas, el comportamiento de colisión y la distribución espacial de tiles en la escena de juego.

## Objetivo técnico

Esta iteración persigue tres objetivos concretos:

- estabilizar el punto en el que cada entidad dinámica crea su cuerpo de colisión;
- hacer que el manager de objetos controle de forma explícita la activación inicial de cada instancia registrada;
- ajustar el contenido y comportamiento de las escenas para aprovechar ese nuevo ciclo de alta.

## Estructura del proyecto

La estructura general no cambia, pero la revisión se concentra en estos puntos:

- `src/WandAllegroEngine/Models/`, donde se agrega una fase de inicialización virtual en la jerarquía base de objetos.
- `src/WandAllegroEngine/Managers/`, donde se redefine el punto de creación de cuerpos y el orden de alta de objetos.
- `src/Entities/`, donde varias entidades trasladan la creación de cuerpos desde el constructor hacia `Init()`.
- `src/Scenes/`, donde cambia el contenido instanciado y la distribución de objetos en `GameInScene`.
- `src/WandAllegroEngine/Collisions/`, donde se amplía el recorrido efectivo de celdas para detección de colisiones.

## Arquitectura o sistemas principales

### Activación de objetos

`GameObject` incorpora `virtual void Init()`, lo que introduce un punto formal para completar la preparación del objeto después de su registro en el manager global.

Ese cambio se apoya en `GameObjectManager::Add()`, que ahora ejecuta este flujo:

1. asigna el identificador del objeto;
2. llama `Init()`;
3. inserta la instancia en la colección administrada;
4. ordena por capa.

Con este ajuste, la activación deja de depender de efectos secundarios en el constructor y pasa a quedar asociada al momento real de incorporación al sistema.

Además, `GameObjectManager` deja inicializado `nextEntityId` desde el constructor y ya no depende de una inicialización inline en el header.

### Creación de cuerpos de colisión

El cambio más importante de esta etapa afecta la creación de cuerpos.

En `LinkObject`, `MapTileObject` y `CustomEnemy`:

- se elimina la creación inmediata del cuerpo dentro del constructor;
- se agrega `Init() override`;
- el cuerpo se registra desde `Init()` mediante `Game->gameBodyManager->Add(this, GetPosition(), GetSize())`.

En `GameBodyManager`:

- la firma de `Add()` cambia y deja de recibir un `GameBodyObject*` preconstruido;
- `Add()` pasa a recibir el `GameObject` y construye internamente el `GameBodyObject`;
- el cuerpo adopta el ID del objeto ya registrado antes de incorporarse al contenedor de cuerpos.

La consecuencia técnica es que el cuerpo nace cuando el objeto ya fue aceptado por `GameObjectManager`, no antes. Eso elimina la dependencia previa entre creación del objeto y existencia de un ID aún no consolidado.

### Ajustes en colisiones

`GameGridCollisions` modifica el recorrido usado en `Find_collision_grid()`:

- el barrido deja de iniciar en índices interiores (`1 ... n-1`);
- el análisis pasa a cubrir desde `0` hasta el borde completo de filas y columnas.

Con este cambio, la detección deja de excluir las celdas periféricas del grid. La revisión de vecinos se mantiene, pero ahora se aplica también sobre los extremos de la grilla siempre que la celda solicitada exista.

### Ajustes en entidades

Además del traslado de la lógica de inicialización, dos entidades reciben cambios de comportamiento visibles.

En `CustomEnemy`:

- `Draw()` muestra ahora el tiempo en una posición distinta;
- se agrega una segunda línea de texto con el nombre del objeto;
- `OnCollision()` incorpora un caso explícito para `Player`, al que transfiere el sprite del enemigo.

En `LinkObject` y `MapTileObject`:

- el cambio principal se concentra en la fase `Init()` y en el abandono del registro de cuerpo desde el constructor.

## Escenas y flujo visible

### `MainScene`

`MainScene` mantiene la estructura general ya consolidada, pero ajusta la nomenclatura efectiva de objetos creados:

- el jugador pasa a registrarse con nombre `Player`;
- el enemigo pasa a registrarse con nombre `Enemy`.

Este detalle afecta la semántica de las colisiones, porque varias comprobaciones dependen directamente del nombre del objeto.

### `GameInScene`

`GameInScene` concentra la mayor parte de los cambios visibles de esta etapa.

- se introduce un alias local para `resourceManager` (`auto GRM = Game->resourceManager`);
- el jugador pasa a crearse con nombre `Player`;
- la cuadrícula regular de tiles queda desactivada en el código visible mediante comentario;
- en su lugar, la escena genera `100` tiles en posiciones aleatorias dentro de una superficie de `1920 x 1080`;
- los tiles se instancian con tamaño `64 x 64`;
- se agrega un atajo con `H` para alternar la depuración de cuerpos también desde esta escena.

El cambio más relevante no es solo visual. La escena deja de producir una distribución uniforme de mosaicos y pasa a poblar el espacio de forma dispersa, lo que modifica la densidad local de colisiones y el uso efectivo del grid.

## Dependencias externas visibles

- **Allegro 5** sigue siendo la biblioteca activa para rendering, eventos, fuentes, audio e input.
- **`nlohmann/json`** continúa presente en `include/`.

## Resumen técnico de la versión

La revisión efectiva al **31 de octubre de 2024** queda definida por estos cambios:

- se incorpora `Init()` como fase explícita de activación en `GameObject`;
- `GameObjectManager::Add()` pasa a disparar esa fase después de asignar el ID;
- la creación de cuerpos se traslada desde constructores concretos hacia `Init()`;
- `GameBodyManager` asume la construcción interna de `GameBodyObject` a partir del objeto registrado;
- `GameGridCollisions` amplía la revisión de colisiones a toda la grilla, incluidos los bordes;
- `GameInScene` sustituye la distribución regular de tiles por una dispersión aleatoria de cien instancias;
- `CustomEnemy` incorpora salida visual adicional y una reacción directa al colisionar con `Player`;
- `MainScene` y `GameInScene` normalizan la nomenclatura visible de objetos hacia `Player` y `Enemy`.

La etapa queda, en conjunto, más alineada con un flujo en el que el manager de objetos controla la entrada real de cada entidad al runtime y en el que la inicialización dependiente del ID se ejecuta en el momento correcto.