# WandAllegroEngine

## Fecha efectiva

**12 de noviembre de 2024**

## Descripción general

En esta etapa reforcé la infraestructura espacial del framework para que el runtime pueda decidir con mayor claridad qué objetos deben actualizarse, dibujarse o quedar fuera del área activa de trabajo. El cambio central ya no está en la persistencia, sino en la coordinación entre cámara, grid de colisiones, objetos y managers base.

Sobre esa misma línea también rehice el tratamiento de los sprite sheets dentro del cargador de recursos para que cada región quede desacoplada del bitmap principal y pueda reutilizarse como recurso independiente dentro del motor.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- controlar visibilidad y actualización de objetos desde el estado espacial del runtime;
- permitir que el grid activo del motor pueda reemplazarse dinámicamente;
- estabilizar el acceso a regiones de sprite sheet como recursos reutilizables.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Models/` y `src/WandAllegroEngine/Managers/`, donde `GameObject`, `ObjectManager`, `BodyManager` y `CameraManager` pasan a compartir estados explícitos de visibilidad, pertenencia al área activa y consulta de cámara.
- `src/WandAllegroEngine/Collisions/`, donde `GameGridCollisions` deja de limitarse a poblar celdas y empieza a marcar qué objetos están dentro del grid y cuáles deben considerarse visibles según la cámara.
- `src/WandAllegroEngine/Managers/ResourceManager.*`, donde el manejo de sprite sheets cambia de regiones calculadas al vuelo a una matriz persistente de bitmaps independientes.

## Arquitectura o sistemas principales

### Visibilidad y pertenencia al área activa

`GameObject` incorpora dos estados nuevos dentro de su contrato base:

- `Visible`;
- `InsideGridArea`.

Sobre esa base también agregué los accesores y mutadores correspondientes:

- `SetVisible()` y `GetVisible()`;
- `SetInsideGridArea()` y `GetInsideGridArea()`.

Con esto el objeto deja de depender únicamente de su existencia en memoria para participar en el frame, y pasa a poder ser filtrado por managers según su presencia dentro del área activa y su visibilidad efectiva.

### Culling espacial desde cámara y grid

`GameGridCollisions` ahora marca explícitamente si un objeto entra o no en el grid activo al momento de insertarlo en celdas. Si el cuerpo no cae dentro del área de la grilla, el objeto asociado queda marcado fuera de `InsideGridArea`; si sí cae dentro, vuelve a marcarse como válido.

Además, durante el recorrido de colisiones, la grilla consulta `CameraManager::IsObjectInsideCameraArea()` para decidir si el objeto debe permanecer visible. De esta forma, el sistema de colisiones también pasa a alimentar el filtrado visual del runtime.

### Managers coordinados por estado espacial

`ObjectManager::Update()` deja de actualizar cualquier actor vivo de forma indiscriminada y pasa a hacerlo solo cuando el objeto sigue dentro del área activa del grid. Del mismo modo, `ObjectManager::Draw()` y `BodyManager::Draw()` filtran por `GetVisible()` antes de renderizar.

En paralelo, `BodyManager` deja de asumir que siempre existe una grilla inicial fija. Ahora puede trabajar sin grid activa, exponer una posición segura cuando no existe una, y recibir una nueva mediante `SetNewGridArea(GameGridCollisions *)`.

Con esto la relación entre cuerpo, cámara, visibilidad y actualización ya no queda dispersa, sino coordinada desde el estado espacial común del runtime.

### API de cámara orientada a consulta espacial

`CameraManager` amplía su interfaz con `IsObjectInsideCameraArea(GameObject *Obj)`, que evalúa si un objeto intersecta el rectángulo visible actual de cámara.

Este cambio complementa la consulta previa de foco y convierte a la cámara en una referencia útil no solo para transformar la vista, sino también para decidir qué contenido del mundo debe seguir activo visualmente.

### Sprite sheets como recursos persistentes

`ResourceManager` cambia su estrategia de sprite sheets. En lugar de conservar un bitmap principal y devolver sub-bitmaps creados en cada consulta, ahora divide la hoja al cargarla y almacena una matriz bidimensional de bitmaps independientes.

Ese ajuste implica tres cambios visibles:

- `loadSpriteSheet()` pasa a trabajar explícitamente con filas y columnas;
- `getBitmapRegionFromSpriteSheet()` devuelve directamente el bitmap persistido en la matriz;
- `Clear()` destruye cada región almacenada como recurso independiente.

Con ello la obtención de sprites queda más estable para reutilización continua dentro del framework y evita depender de sub-bitmaps temporales creados en cada acceso.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- el runtime puede omitir actualización y dibujo de objetos fuera del área útil sin destruirlos ni sacarlos del flujo general del motor;
- los recursos gráficos extraídos de sprite sheets ya quedan listos para reutilización directa desde managers, escenas y utilidades de edición.

Con ello dejé una base más útil para trabajar con mundos extensos, herramientas de edición y selección visual sin cargar de trabajo innecesario el frame ni recalcular regiones gráficas en cada acceso.

## Dependencias externas visibles

No incorporé dependencias externas visibles nuevas en esta etapa.

## Resumen técnico de la versión

La etapa efectiva al **12 de noviembre de 2024** queda delimitada por estos movimientos:

- incorporé estados explícitos de visibilidad y pertenencia al grid dentro de `GameObject`;
- hice que `ObjectManager`, `BodyManager` y `GameGridCollisions` utilicen esos estados para filtrar actualización, dibujo y presencia espacial;
- amplié `CameraManager` con una consulta directa para saber si un objeto está dentro del área visible actual;
- permití reemplazar dinámicamente la grilla activa desde `BodyManager` en lugar de asumir una instancia fija permanente;
- rehice el almacenamiento de sprite sheets en `ResourceManager` para conservar regiones independientes ya separadas desde la carga.

Con esta etapa dejé el framework más preparado para edición, navegación espacial y administración eficiente de recursos visuales dentro del runtime.