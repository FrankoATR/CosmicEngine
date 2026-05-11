# WandAllegroEngine

## Fecha efectiva

**4 de noviembre de 2024**

## Descripción general

En esta etapa reforcé el cierre del runtime y completé varias piezas que todavía estaban incompletas en la capa de managers. El cambio principal consiste en que la destrucción de escenas, recursos, eventos, objetos y cuerpos queda ahora más explícita y consistente entre managers, en lugar de depender de limpiezas parciales o implícitas.

Sobre esa base también incorporé consultas directas en `ObjectManager` para localizar objetos por identidad lógica, con el fin de que el framework pueda resolver transiciones o validaciones de estado sin exigir estructuras auxiliares externas.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- cerrar correctamente el ciclo de vida de escenas y managers compartidos;
- exponer búsquedas directas de objetos dentro del manager central de entidades;
- homogeneizar la interfaz de limpieza entre managers para reducir ambigüedad operativa.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Managers/`, donde ajusté `SceneManager`, `ObjectManager`, `BodyManager`, `ResourceManager` y `EventManager` para que su limpieza y destrucción sean más uniformes.
- `src/WandAllegroEngine/Models/`, donde `GameScene` amplía el alcance de `Clear()` para vaciar también el sistema de eventos además de cuerpos, objetos y recursos.
- `src/Scenes/`, donde las escenas ya pueden apoyarse en consultas directas del framework para decidir transiciones según el estado actual de los objetos registrados.

## Arquitectura o sistemas principales

### Ciclo de vida de escenas

En `SceneManager` completé la liberación efectiva de escenas al reemplazar, extraer o vaciar la pila. Después de ejecutar `Clear()` sobre la escena activa, ahora también se destruye explícitamente la instancia antes de retirarla del contenedor.

Con este ajuste evité que la limpieza lógica de una escena quedara separada de su liberación real en memoria.

### Limpieza coordinada de managers

En `GameScene::Clear()` amplié la limpieza del runtime para incluir también `EventManager`, además de `BodyManager`, `ObjectManager` y `ResourceManager`.

Al mismo tiempo normalicé la interfaz de recursos desde `clear()` hacia `Clear()` y ajusté `BodyManager`, `ObjectManager`, `ResourceManager`, `EventManager` y `GameManager` para que sus rutinas de cierre utilicen una terminología y una secuencia de destrucción más consistentes.

### Consultas sobre objetos registrados

En `ObjectManager` incorporé dos búsquedas directas:

- `FindById(int EntityId)`;
- `FindByUniqueName(std::string UniqueName)`.

Con esto dejé disponible una vía interna para consultar el estado del conjunto de objetos administrados sin recorrer manualmente la colección desde cada consumidor del framework.

### Vaciado explícito de contenedores

En varios managers sustituí la limpieza basada en iteración simple por un vaciado explícito desde el final del contenedor mediante `while (!container.empty())`, destruyendo cada elemento antes de retirarlo.

Ese patrón queda visible en `BodyManager::Clear()`, `ObjectManager::Clear()` y `EventManager::Clear()`, y refuerza un cierre más directo del estado administrado por cada servicio.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- las escenas pueden resolver transiciones a partir de consultas directas al estado de los objetos administrados;
- el cierre de escena vacía de forma conjunta los subsistemas compartidos que quedaron asociados a su ejecución.

Con ello dejé una base más útil para escenas que dependen del estado actual del runtime y para transiciones que requieren limpiar completamente el contexto anterior antes de continuar.

## Dependencias externas visibles

No incorporé dependencias externas visibles nuevas en esta etapa.

## Resumen técnico de la versión

La etapa efectiva al **4 de noviembre de 2024** queda delimitada por estos movimientos:

- completé la destrucción explícita de escenas dentro de `SceneManager` al reemplazar, extraer o limpiar la pila;
- amplié `GameScene::Clear()` para vaciar también `EventManager` junto con cuerpos, objetos y recursos;
- unifiqué la operación de limpieza de recursos bajo `ResourceManager::Clear()`;
- incorporé `FindById()` y `FindByUniqueName()` en `ObjectManager` como consultas directas sobre entidades registradas;
- reforcé el vaciado explícito de contenedores en managers que destruyen objetos, cuerpos y eventos.

Con esta etapa dejé el framework más consistente en cierre, limpieza y consulta de estado interno, especialmente en transiciones de escena y en ciclos donde el contexto anterior debe desaparecer por completo antes de continuar.