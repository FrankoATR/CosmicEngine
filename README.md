# WandAllegroEngine

## Fecha efectiva

**17 de noviembre de 2024**

## Descripción general

En esta etapa llevé el framework hacia un soporte más claro para herramientas de edición y manipulación interactiva del runtime. El cambio más visible es que la capa UI deja de tratar todos los elementos como equivalentes y pasa a distinguir tipos concretos, mientras el sistema de entrada separa la posición absoluta del cursor respecto a la posición transformada por cámara.

Sobre esa misma base también abrí operaciones nuevas sobre el conjunto administrado de objetos, incorporé clonación en el modelo base y reemplacé la persistencia anterior por una implementación apoyada en SQLite para guardar estado de objetos del motor.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- tipar los elementos de interfaz para refinar su comportamiento dentro del framework;
- separar coordenadas de entrada de pantalla y de mundo para soportar edición con cámara;
- ampliar el acceso, clonación y persistencia de objetos administrados por el runtime.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Interfaces/` y `src/WandAllegroEngine/Models/`, donde `Definitions.hpp` incorpora `UIElementType` y `UIElement` pasa a conservar el tipo concreto de cada elemento de interfaz.
- `src/WandAllegroEngine/Managers/`, donde `InputManager` y `ObjectManager` amplían su contrato para selección espacial, lectura de cursor absoluto y administración más directa del conjunto de objetos.
- `src/Utilities/`, donde `DataManager` abandona el guardado simple anterior y pasa a una base SQLite para persistencia de `GameObject`.

## Arquitectura o sistemas principales

### Tipado explícito de elementos UI

`Definitions.hpp` incorpora `UIElementType` con categorías visibles para `Button`, `Label` e `Image`, y `UIElement` pasa a almacenar ese tipo como parte de su estado base. Desde esta etapa el constructor del elemento ya no solo recibe posición, tamaño y visibilidad, sino también la categoría concreta del nodo de interfaz.

Ese ajuste se refleja de inmediato en los elementos actuales del framework:

- `UIButton` se registra como `UIElementType::Button`;
- `UIText` se registra como `UIElementType::Label`.

Con ello la capa UI deja de depender únicamente de comportamiento implícito por clase derivada y gana una clasificación común reutilizable por managers y herramientas.

### Interacción refinada dentro de UIManager

Sobre esa nueva clasificación, `UIManager::Update()` deja de considerar todos los elementos como candidatos de hover. A partir de esta etapa, los elementos marcados como `Label` ya no activan `MouseHoverAny`, con lo que el texto puede convivir dentro de la interfaz sin bloquear la interacción destinada a controles realmente interactivos.

Además, `UIButton` agrega una representación visible del hover mediante un contorno dibujado sobre su área efectiva, reforzando la retroalimentación del framework para edición y navegación por interfaz.

### Entrada separada entre pantalla y mundo

`InputManager` amplía su contrato con `GetAbsoluteMousePosition() const`, que devuelve la posición del cursor en coordenadas de pantalla sin aplicar el desplazamiento de cámara.

Este cambio convive con `GetMousePosition() const`, que sigue devolviendo la posición transformada al espacio del mundo. Con ambas variantes disponibles, el framework ya puede sostener simultáneamente herramientas de interfaz fija y selección espacial dentro de escenas desplazables.

### Consultas y clonación sobre objetos administrados

`ObjectManager` deja de ofrecer solo búsquedas por identidad o nombre y agrega consultas directamente útiles para herramientas:

- `FindByPosition(WAND_VEC2 Position)`;
- `FindByMousePosition()`;
- `FindByLayer(int LayerId)`;
- `GetAll()`.

En paralelo, `GameObject` incorpora un constructor basado en otro objeto y expone `Clone() const`, creando una ruta visible para duplicar entidades administradas a partir de su estado actual.

Con ello el runtime gana soporte directo para selección, duplicación e inspección de objetos sin tener que rearmar manualmente esas operaciones en cada escena o utilidad.

### Persistencia apoyada en SQLite

`DataManager` abandona el esquema anterior de guardado simple y pasa a trabajar con `sqlite3`. La nueva implementación visible:

- abre `./saves/game_data.db` al inicializarse;
- crea una tabla `GameObjects` si todavía no existe;
- expone `ClearGameObjects()` para vaciar la tabla;
- guarda objetos mediante `SaveData(const GameObject &obj)`;
- reconstruye un conjunto de objetos mediante `LoadData()`.

Con esto la persistencia deja de estar limitada a un par de valores sueltos y pasa a modelarse alrededor de registros de objetos del motor.

### Separación de responsabilidades espaciales

`GameGridCollisions` deja de ocuparse de marcar visibilidad según cámara y ese criterio pasa a concentrarse en `ObjectManager::Update()` mediante `CameraManager::IsObjectInsideCameraArea(actor)`.

El efecto estructural es que la grilla vuelve a centrarse en validación y colisión, mientras el manager de objetos asume la responsabilidad de decidir visibilidad efectiva en el runtime.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- la capa UI puede distinguir controles interactivos frente a elementos puramente informativos sin mezclar ambas responsabilidades en el mismo flujo de hover;
- las herramientas del runtime ya disponen de selección por posición, clonación de objetos y persistencia estructurada sobre una base de datos local.

Con ello dejé una base más útil para editores, inspección de escena y utilidades de manipulación en tiempo de ejecución, con una separación más clara entre interfaz, entrada, selección y persistencia.

## Dependencias externas visibles

Se incorpora una dependencia externa visible nueva en esta etapa: `sqlite3`, utilizada desde `DataManager` para persistencia local de objetos del framework.

## Resumen técnico de la versión

La etapa efectiva al **17 de noviembre de 2024** queda delimitada por estos movimientos:

- incorporé `UIElementType` como clasificación común para los elementos de interfaz del framework;
- adapté `UIElement`, `UIButton` y `UIText` a esa clasificación y evité que las etiquetas activen estado de hover interactivo;
- añadí `GetAbsoluteMousePosition()` a `InputManager` para separar coordenadas de pantalla y coordenadas de mundo;
- amplié `ObjectManager` con búsquedas por posición, por mouse, por capa y lectura completa del conjunto administrado;
- incorporé clonación visible en `GameObject` mediante constructor derivado y `Clone() const`;
- migré `DataManager` a una base SQLite local para persistencia de objetos;
- trasladé la decisión de visibilidad por cámara desde `GameGridCollisions` hacia `ObjectManager`.

Con esta etapa dejé el framework más preparado para edición interactiva, selección de objetos y persistencia estructurada dentro del runtime.