# WandAllegroEngine

## Fecha efectiva

**19 de noviembre de 2024**

## Descripción general

En esta etapa terminé de volver utilizable la infraestructura de temporización incorporada previamente. El cambio ya no está solo en tener timers registrados por un manager, sino en que `GameTimer` corrige su arranque para no dispararse de forma inmediata al crearse y pasa a sostener comportamientos repetitivos y temporales con una referencia inicial de tiempo coherente.

Sobre esa misma base el framework ya empieza a absorber patrones más concretos de temporización, como esperas entre acciones y vidas útiles limitadas, sin volver a depender de comparaciones manuales dispersas contra `al_get_time()`.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- estabilizar el comportamiento inicial de los timers del framework;
- validar el uso de temporización para cadencias y expiración de objetos;
- seguir desplazando lógica temporal manual hacia un servicio reutilizable del motor.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Models/GameTimer.*`, donde la lógica interna del timer se ajusta para inicializar correctamente su referencia temporal al primer `Update()`.
- `src/Entities/`, donde la capa de timers empieza a sostener tanto esperas entre acciones como expiración automática de objetos temporales.
- `src/Utilities/DataManager.cpp`, donde se ajusta la integración con persistencia SQLite sin cambiar el contrato público del servicio.

## Arquitectura o sistemas principales

### Arranque consistente de GameTimer

`GameTimer` incorpora ahora un estado interno `Init` y utiliza el primer `Update(double CurrentTime)` para fijar `LastTimeTrigger` antes de evaluar el umbral de espera.

El efecto práctico es que un timer deja de comportarse como si ya hubiera estado corriendo desde tiempo cero. Con ello el primer disparo ocurre después del intervalo configurado y no inmediatamente al registrarse en el runtime.

Ese ajuste vuelve utilizable la semántica temporal del framework para cooldowns, loops y expiraciones donde el instante de creación del timer debe considerarse el verdadero punto de partida.

### Temporización aplicada a cadencias y vida útil

La nueva semántica ya se aprovecha en dos patrones concretos que validan la capa de timers:

- esperas entre acciones repetidas, donde un timer puede activarse, pausarse y reiniciarse para controlar una cadencia;
- vida útil limitada de objetos temporales, donde un timer no cíclico puede marcar el momento de finalización del objeto.

Aunque esos casos visibles hoy aparecen consumidos desde entidades concretas, lo importante es que el framework ya demuestra dos usos distintos de la misma infraestructura temporal sin lógica manual adicional.

### TimerManager como coordinador operativo

`TimerManager` mantiene el rol central introducido previamente, pero en esta etapa queda validado como coordinador operativo de timers reales que dependen de inicialización diferida, repetición y finalización.

La combinación entre `Add(GameTimer *NewTimer)`, `Update(double CurrentTime)` y `Clear()` ya no queda solo como contrato base, sino como flujo suficiente para sostener timers activos ligados al ciclo completo del runtime.

### Ajustes menores de integración

En la capa de persistencia se corrige la ubicación del include de `LinkObject` dentro de `DataManager.cpp`, manteniendo la integración con SQLite sin alterar la interfaz pública del servicio.

Además, el modelo base conserva `Rotation` como campo preparado para extensiones futuras, pero en esta iteración el cambio dominante sigue siendo la estabilización funcional del sistema de timers.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- el runtime ya puede crear timers sin que se disparen de forma anticipada al primer frame útil;
- la misma infraestructura temporal ya sirve tanto para intervalos repetitivos como para expiración de objetos temporales.

Con ello dejé una base más útil para cooldowns, proyectiles, IA simple y futuros eventos diferidos, con una semántica temporal más consistente dentro del motor.

## Dependencias externas visibles

No incorporé dependencias externas visibles nuevas en esta etapa.

## Resumen técnico de la versión

La etapa efectiva al **19 de noviembre de 2024** queda delimitada por estos movimientos:

- corregí el arranque interno de `GameTimer` para que el primer disparo respete el tiempo de espera configurado desde su creación;
- validé la infraestructura temporal del framework para manejar tanto cadencias repetitivas como vida útil limitada de objetos temporales;
- mantuve la integración de `TimerManager` con el loop principal y el cierre de escena como base operativa de esa temporización;
- ajusté la integración de `DataManager` con SQLite sin modificar su contrato público;
- conservé `Rotation` en `GameObject` como preparación de estado adicional para extensiones posteriores.

Con esta etapa dejé el framework más sólido para lógica basada en tiempo y para objetos que necesitan cadencia o expiración controlada dentro del runtime.