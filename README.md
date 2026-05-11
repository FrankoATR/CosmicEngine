# WandAllegroEngine

## Fecha efectiva

**5 de noviembre de 2024**

## Descripción general

En esta etapa consolidé dos piezas nuevas del framework: un sistema de eventos tipado que ya no se limita a callbacks sin argumentos y un manager dedicado para reproducción musical persistente. Con eso el motor pasa a poder coordinar reacciones entre subsistemas usando parámetros tipados y también separar el audio musical del arranque general del juego.

Sobre esa base también ajusté varios puntos de infraestructura menor para que esos servicios nuevos se integren mejor en el ciclo de vida del runtime.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- convertir el sistema de eventos en un mecanismo genérico con parámetros tipados;
- incorporar un servicio específico para carga y reproducción de música;
- redistribuir responsabilidades de audio y estado interno para que el framework no dependa de un único punto de inicialización monolítico.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Interfaces/`, donde `GameEvent.hpp` desaparece y se incorporan `IEvent.hpp` y `Event.hpp` como base del nuevo sistema de eventos genéricos.
- `src/WandAllegroEngine/Managers/`, donde `EventManager` pasa a almacenar eventos tipados, `MusicManager` entra en uso efectivo y `GameManager` deja de inicializar directamente la capa de audio musical.
- `src/WandAllegroEngine/Models/`, donde `GameObject` agrega mutación explícita del nombre y `GameScene` incorpora limpieza del manager de música dentro del cierre de escena.

## Arquitectura o sistemas principales

### Sistema de eventos tipado

En `EventManager` reemplacé el registro anterior por un contenedor de eventos polimórficos apoyado en dos nuevas bases:

- `IEvent`, como interfaz mínima para almacenamiento heterogéneo;
- `Event<Args...>`, como plantilla que conserva y ejecuta listeners con firma tipada.

Con esa base, `EventManager` pasa a registrar y disparar eventos mediante plantillas:

- `RegisterEvent(const std::string &eventName, std::function<void(Args...)> callback)`;
- `TriggerEvent(const std::string &eventName, Args... args)`;
- `RemoveEvent(const std::string &eventName)`.

El efecto concreto es que el framework ya puede transportar argumentos tipados dentro del sistema de eventos y validar en tiempo de ejecución si la firma registrada coincide con la firma usada al disparar el evento.

### Retiro del contrato anterior de eventos

Como consecuencia del cambio anterior, `GameEvent.hpp` sale del árbol activo y deja de ser la base del sistema. El modelo anterior, apoyado en objetos con contrato fijo de notificación, queda sustituido por eventos genéricos especializados por plantilla.

Con ello el framework deja de forzar una sola firma de callback para todos los casos y gana un mecanismo más flexible para coordinar subsistemas con necesidades distintas.

### Servicio de música

`MusicManager` entra en uso efectivo como servicio singleton para música continua del runtime. Su interfaz visible queda formada por:

- `LoadMusic(const std::string &name, const std::string &filename)`;
- `PlayMusic(const std::string &name, float volume = 1.0f, bool loop = true)`;
- `PauseMusic()`;
- `ResumeMusic()`;
- `StopMusic()`;
- `Clear()`.

La implementación inicializa voz, mixer, reserva de muestras y flujos de audio, y destruye esos recursos al cerrar el manager. Con esto separé la reproducción musical del arranque general de `GameManager` y dejé un punto específico para administrar pistas persistentes.

### Ajustes de integración base

Para acompañar el cambio de eventos y música, `GameScene::Clear()` también limpia `MusicManager`, `GameObject` incorpora `SetObjectName(std::string NewName)` y `GameManager` deja de instalar directamente audio y codecs en su secuencia de inicialización.

Estos cambios no introducen subsistemas nuevos por sí mismos, pero sí completan la integración del sistema tipado de eventos y del nuevo manager de música dentro del ciclo de vida del framework.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- el sistema de eventos ya puede coordinar interacciones que requieren argumentos sin limitarse a notificaciones vacías;
- la reproducción musical queda administrada desde un servicio específico que puede cargarse, iniciarse, pausarse o limpiarse sin mezclar esa lógica con el manager principal del juego.

Con ello dejé una base más útil para extender la comunicación entre objetos y escenas, y también para sostener audio persistente como parte del runtime sin acoplarlo al flujo central de inicialización.

## Dependencias externas visibles

No incorporé dependencias externas visibles nuevas en esta etapa.

## Resumen técnico de la versión

La etapa efectiva al **5 de noviembre de 2024** queda delimitada por estos movimientos:

- sustituí la base anterior de eventos por una arquitectura tipada apoyada en `IEvent` y `Event<Args...>`;
- convertí `EventManager` en un registro de eventos genéricos capaces de recibir parámetros al dispararse;
- retiré `GameEvent.hpp` del flujo activo del framework;
- integré `MusicManager` como servicio efectivo para carga, reproducción, pausa, reanudación y limpieza de pistas musicales;
- amplié `GameScene::Clear()` para que el ciclo de cierre de escena también vacíe el manager de música;
- añadí `SetObjectName()` a `GameObject` como ajuste de mutabilidad básica para objetos administrados por el motor;
- retiré de `GameManager` la inicialización directa de audio musical para redistribuir esa responsabilidad.

Con esta etapa dejé el framework más flexible para comunicación tipada entre subsistemas y mejor preparado para manejar música persistente como parte de la infraestructura general del runtime.