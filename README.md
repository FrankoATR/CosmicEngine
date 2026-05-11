# WandAllegroEngine

## Fecha efectiva

**13 de noviembre de 2024**

## Descripción general

En esta etapa amplié la capa de interfaz del framework para que ya no se limite a botones y pueda sostener también texto renderizado y cursores personalizados controlados desde el runtime. A partir de este punto la interacción visual deja de depender de atajos dentro de escenas concretas y pasa a tener soporte explícito desde los managers base de entrada y dibujo.

Sobre esa misma base también ajusté la validación espacial del grid para considerar tamaño real de cuerpos, y abrí nuevas consultas sobre colecciones de objetos administrados para facilitar herramientas y flujos de inspección dentro del motor.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- extender la capa UI con texto y cursor administrado desde el framework;
- endurecer la validación de pertenencia al grid usando posición y tamaño del cuerpo;
- exponer consultas más directas sobre los objetos administrados por el runtime.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Models/UIElements/`, donde se incorpora `UIText` como segundo elemento visible de la capa UI del framework.
- `src/WandAllegroEngine/Managers/` y `src/WandAllegroEngine/Models/`, donde `InputManager`, `GameScene`, `ObjectManager` y `GameObject` amplían su contrato para soportar cursor gráfico, consultas de objetos y lectura de última posición.
- `src/WandAllegroEngine/Collisions/`, donde `GameGridCollisions` sustituye la validación por punto por una validación basada en posición y tamaño del cuerpo dentro del grid.

## Arquitectura o sistemas principales

### Texto nativo dentro de la capa UI

Agregué `UIText` como nuevo elemento base de interfaz. Su implementación visible resuelve:

- contenido textual mutable mediante `SetText()`;
- fuente configurable con `SetFont()`;
- color lógico de texto con `SetTextColor()`;
- dibujo centrado dentro del área ocupada por el elemento.

Con ello la UI del framework deja de depender únicamente de sprites interactivos y pasa a poder representar información textual persistente desde la misma jerarquía de `UIElement`.

### Cursor gráfico administrado por InputManager

`InputManager` amplía su responsabilidad y pasa a controlar un cursor visual propio del framework. La interfaz nueva queda formada por:

- `SetMouseSprite()`;
- `SetMouseSpriteOffSet()`;
- `SetMouseSpriteSize()`;
- `DrawMouseSprite()`;
- `ResetMouseSettings()`.

Esta capa oculta o restaura el cursor nativo según exista un sprite configurado, conserva tamaño y desplazamiento del cursor gráfico, y deja el dibujo integrado al ciclo visual del runtime.

En la misma línea, `GameScene::Draw()` ahora invoca `InputManager::GetInstance().DrawMouseSprite()` y `GameScene::Clear()` restablece la configuración de mouse al cerrar la escena.

### Validación espacial por tamaño real

`GameGridCollisions` deja de decidir pertenencia al grid usando solo un punto y pasa a evaluar posición y tamaño completos del cuerpo con `GetCellByPositionAndSize()`.

El cambio visible incluye tres efectos:

- la grilla conserva un `GridSize` explícito;
- un cuerpo que rebasa los límites queda fuera del área activa;
- cuando eso ocurre, el objeto asociado vuelve a `SetToLastPosition()` en lugar de seguir avanzando fuera del espacio válido.

Con ello el grid deja de aceptar cuerpos parcialmente fuera de rango como si todavía ocuparan una celda válida.

### Consultas y estado de objetos administrados

`ObjectManager` incorpora dos consultas nuevas:

- `FindByLayer(int LayerId)`;
- `GetAll()`.

Además, `GameObject` expone `GetLastPosition() const`, lo que completa el acceso de lectura al estado espacial previo del objeto desde otras capas del framework.

Con esto el runtime gana una base más útil para inspección, herramientas de edición y lógica auxiliar que necesita observar el conjunto administrado sin acoplarse a detalles internos del manager.

### Ajustes de robustez en recursos

En `ResourceManager` corregí la validación de índices de `getBitmapRegionFromSpriteSheet()` para que la comprobación de columnas use el tamaño real de la fila consultada.

No cambia el contrato principal del manager, pero sí evita lecturas inválidas cuando la hoja cargada no debe tratarse como una matriz cuadrada.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- la escena activa ya puede renderizar un cursor gráfico y texto UI dentro del mismo flujo donde dibuja objetos, depuración visual e interfaz;
- la validación espacial del grid puede rechazar desplazamientos fuera de rango y conservar una última posición consistente para los objetos administrados.

Con ello dejé una base más útil para menús, herramientas de edición y navegación interactiva, con una capa UI más completa y un control espacial más estricto desde el runtime.

## Dependencias externas visibles

No incorporé dependencias externas visibles nuevas en esta etapa.

## Resumen técnico de la versión

La etapa efectiva al **13 de noviembre de 2024** queda delimitada por estos movimientos:

- incorporé `UIText` como nuevo elemento visible de la capa UI del framework;
- extendí `InputManager` para administrar cursor gráfico, su configuración visual y su restauración al cerrar escena;
- integré el dibujo del cursor al flujo base de `GameScene`;
- reemplacé la validación de celda por una validación basada en posición y tamaño completos dentro de `GameGridCollisions`;
- añadí `GetLastPosition()` a `GameObject` y nuevas consultas de colección a `ObjectManager` para acceso por capa y lectura completa de actores;
- corregí la validación de índices de sprite sheet en `ResourceManager`.

Con esta etapa dejé el framework más preparado para interfaz dinámica, herramientas visuales y control espacial consistente dentro del runtime.