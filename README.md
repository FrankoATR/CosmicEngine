# WandAllegroEngine

## Fecha efectiva

**12 de noviembre de 2024**

## Descripción general

En esta etapa empecé a mover el framework hacia una base de persistencia y serialización propia para datos simples del runtime. El cambio más visible es que los tipos espaciales compartidos dejan de ser solamente estructuras de uso interno y pasan a quedar preparados para representarse en JSON, lo que abre una ruta directa para guardar y reconstruir estado sin depender de conversiones externas dispersas.

Junto con eso añadí una utilidad específica para lectura y escritura de datos en disco, reforcé algunos puntos de la API base de cámara y objetos, y dejé reservado el espacio estructural para futuros managers orientados a temporización, agenda de tareas y animación de sprites.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- preparar serialización y persistencia básica para datos del runtime;
- ampliar la API base de cámara y objetos para consumo más seguro desde otras capas;
- dejar estructurada la siguiente expansión de managers especializados del framework.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Interfaces/`, donde `Definitions.hpp` deja preparada la serialización JSON de `WAND_VEC2` y ajusta su construcción por defecto para soportar esa ruta de datos.
- `src/Utilities/`, donde aparecen `DataManager` y `GameData` como base de persistencia y representación serializable de datos del juego.
- `src/WandAllegroEngine/Managers/`, donde se amplía la interfaz de cámara y se agregan los archivos base de `TimerManager`, `ScheduleManager` y `SpriteAnimationManager` como preparación estructural de siguientes subsistemas.

## Arquitectura o sistemas principales

### Serialización de tipos base

`Definitions.hpp` deja de limitarse a definir estructuras compartidas y pasa a incluir la especialización `adl_serializer` de `nlohmann::json` para `WAND_VEC2`. Con ello el vector base del framework ya puede transformarse de forma directa a una representación JSON con claves `x` e `y`, y reconstruirse desde esa misma estructura.

Para sostener esa ruta también incorporé un constructor por defecto en `WAND_VEC2`, lo que permite que el tipo pueda existir como destino válido durante procesos de deserialización y carga de datos.

### Persistencia básica de runtime

`DataManager` entra como utilidad dedicada para guardar y recuperar datos simples del estado del juego. Su interfaz visible queda concentrada en:

- `LoadData(int &PositionX, int &PositionY)`;
- `SaveData(int PositionX, int PositionY)`.

La implementación usa archivos JSON sobre una carpeta `saves` ubicada junto al ejecutable, crea el directorio cuando hace falta y elimina el archivo si detecta un contenido inválido. Con esto queda establecida una primera base para persistencia local desde el propio framework.

### Estructura serializable de datos

Además de la utilidad de guardado, agregué `GameData` como estructura de datos con serialización visible basada en `nlohmann::json`. En su estado actual conserva el campo `Type` y deja preparada la incorporación posterior de colecciones de entidades.

Aunque todavía no es una capa completa de guardado de escenas u objetos, sí establece el contrato inicial para representar datos del motor en una forma intercambiable y persistible.

### Ajustes de API en cámara y objetos

`CameraManager` amplía su interfaz con `GetFocusPosition()`, permitiendo consultar el punto central actualmente enfocado en lugar de operar únicamente sobre el desplazamiento bruto de cámara.

En paralelo, `GameObject` vuelve const-correct varios accesores visibles:

- `GetPosition()`;
- `GetSize()`;
- `GetObjectName()`;
- `GetObjectType()`;
- `GetObjectId()`;
- `GetLayerId()`;
- `GetSprite()`;
- `GetAliveInGameManager()`.

Con esto la API base de objetos queda más clara para lectura desde otras capas sin sugerir mutación implícita del estado.

### Preparación de managers futuros

Dentro de `src/WandAllegroEngine/Managers/` dejé creados los archivos base de `TimerManager`, `ScheduleManager` y `SpriteAnimationManager`. En esta etapa todavía funcionan como estructura reservada y no como subsistemas operativos, pero marcan la dirección inmediata de expansión del framework hacia temporización, agenda interna y animación desacoplada.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- los tipos espaciales compartidos ya pueden cruzar la frontera entre memoria y archivo en una forma serializable estándar;
- la base de objetos y cámara expone lecturas más estables para utilidades, herramientas y futuras capas de edición o persistencia.

Con ello dejé una base más útil para avanzar hacia guardado de estado, herramientas auxiliares y subsistemas temporales sin volver a redefinir contratos de datos en cada capa del motor.

## Dependencias externas visibles

No incorporé dependencias externas visibles nuevas en esta etapa, pero `nlohmann::json` pasa a quedar integrado de forma explícita en la serialización de tipos base y estructuras de datos auxiliares.

## Resumen técnico de la versión

La etapa efectiva al **12 de noviembre de 2024** queda delimitada por estos movimientos:

- preparé `WAND_VEC2` para serialización JSON directa y añadí su constructor por defecto para soportar carga de datos;
- incorporé `DataManager` como utilidad visible para guardar y recuperar estado simple en archivos JSON locales;
- agregué `GameData` como estructura serializable inicial para datos del juego;
- amplié `CameraManager` con `GetFocusPosition()` para consultar el centro actual de enfoque;
- volví const-correct la lectura pública de `GameObject` en varios accesores base;
- dejé creada la estructura inicial de `TimerManager`, `ScheduleManager` y `SpriteAnimationManager` como preparación de la siguiente expansión del framework.

Con esta etapa dejé el framework más cerca de una infraestructura de persistencia y herramientas, con contratos de datos más utilizables y una base más clara para subsistemas especializados posteriores.