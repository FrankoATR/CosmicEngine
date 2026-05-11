# WandAllegroEngine

## Fecha efectiva

**19 de noviembre de 2024**

## Descripción general

En esta etapa incorporé una infraestructura real de temporización dentro del framework y la integré al ciclo principal de ejecución. A partir de este punto el motor ya no depende únicamente de comprobaciones manuales con `al_get_time()` dispersas en objetos concretos, sino que cuenta con timers administrados desde un manager dedicado y actualizados en el loop central del runtime.

Sobre esa misma base también dejé la limpieza de timers integrada al cierre de escena y empecé a desplazar parte de la lógica temporal de entidades hacia esta nueva capa reutilizable.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- introducir una infraestructura de timers reutilizable dentro del framework;
- integrar esa temporización al loop principal y al cierre de escena;
- preparar la migración de lógicas temporales dispersas hacia un servicio común del motor.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Models/`, donde aparece `GameTimer` como unidad base para temporización configurable dentro del runtime.
- `src/WandAllegroEngine/Managers/`, donde `TimerManager` entra en uso efectivo y se integra con `GameManager` y `GameScene`.
- `src/Entities/`, donde algunos comportamientos temporales comienzan a apoyarse en la nueva capa de timers en lugar de depender de contadores manuales locales.

## Arquitectura o sistemas principales

### Unidad base de temporización

Agregué `GameTimer` como modelo base para representar timers dentro del framework. Su interfaz visible incorpora:

- `Update(double CurrentTime)`;
- `IsTrigger()`;
- `SetWaitTime(double NewWaitTime)`;
- `GetWaitTime()`;
- `HaveLoop()`;
- `Reset()`;
- `Pause()`;
- `Play()`;
- `End()`;
- `IsEnded()`.

Con esta clase el motor puede manejar esperas configurables, timers repetitivos o de una sola ejecución, pausa y finalización explícita sin reimplementar ese control en cada objeto que lo necesita.

### TimerManager integrado al runtime

`TimerManager` deja de ser solo un placeholder y entra como servicio efectivo para administrar instancias de `GameTimer`. Su contrato visible queda concentrado en:

- `Add(GameTimer *NewTimer)`;
- `Update(double CurrentTime)`;
- `Clear()`.

La actualización recorre todos los timers registrados, ejecuta su estado temporal y elimina de la colección aquellos que ya terminaron. Con ello el framework ya dispone de un punto central para coordinar temporización en lugar de dejarla acoplada a cada entidad.

### Integración al loop principal

La integración visible ocurre en dos puntos del framework:

- `GameManager::Update()` ahora invoca `TimerManager::GetInstance().Update(currentTime)` dentro del loop central;
- `GameScene::Clear()` ahora incorpora `TimerManager::GetInstance().Clear()` dentro del cierre de escena.

Esto vuelve a los timers parte formal del ciclo de vida del runtime: se actualizan en cada frame desde el manager principal y se limpian cuando la escena termina.

### Primer uso real desde entidades

La nueva infraestructura ya empieza a reemplazar lógica temporal manual en objetos del juego. El caso visible más claro es `CustomEnemy`, que ahora registra un `GameTimer` para decidir cuándo cambiar de dirección en lugar de depender de comparaciones locales con `al_get_time()` y contadores manuales.

Aunque ese uso concreto pertenece a una entidad, demuestra que la capa de timers ya es funcional y consumible desde objetos administrados por el framework.

### Preparación de estado temporal en modelos base

Dentro de `GameObject` aparece además un nuevo campo `Rotation`, que deja preparada la base del modelo para futuras extensiones donde la orientación visual o lógica del objeto necesite persistirse junto con el resto de su estado espacial.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- el loop principal del motor ya puede coordinar temporización reutilizable sin delegar toda la lógica temporal a cada entidad por separado;
- el cierre de escena ya contempla también la limpieza de timers activos como parte del ciclo normal de recursos del framework.

Con ello dejé una base más útil para comportamientos programados por tiempo, IA simple, animaciones futuras y eventos diferidos dentro del runtime, con una infraestructura temporal ya integrada al motor.

## Dependencias externas visibles

No incorporé dependencias externas visibles nuevas en esta etapa.

## Resumen técnico de la versión

La etapa efectiva al **19 de noviembre de 2024** queda delimitada por estos movimientos:

- incorporé `GameTimer` como unidad base de temporización configurable para el framework;
- activé `TimerManager` como servicio efectivo para registrar, actualizar y limpiar timers del runtime;
- integré la actualización de timers al loop principal de `GameManager`;
- amplié el cierre de `GameScene` para vaciar también el manager de timers;
- empecé a sustituir control temporal manual en entidades por timers administrados desde el framework;
- añadí `Rotation` a `GameObject` como preparación de estado adicional para futuras extensiones del modelo.

Con esta etapa dejé el framework más preparado para manejar lógica basada en tiempo desde una infraestructura común y reutilizable dentro del motor.