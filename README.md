# WandAllegroEngine

## Fecha efectiva

**7 de noviembre de 2024**

## Descripción general

En esta etapa reorganicé varios tipos fundamentales del framework y los concentré en una base común para que managers, colisiones y modelos dejaran de redefinir estructuras equivalentes en distintos puntos del motor. A partir de ese ajuste, la infraestructura principal pasó a compartir una misma definición de vectores, tamaños, colores, tipos de cuerpo y tipos de objeto.

Sobre esa base también dejé operativa una cámara real dentro del ciclo de dibujo y reestructuré el manejo del grid de colisiones para que el cuerpo central del framework pueda desplazarlo y sincronizarlo con la transformación de cámara.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- unificar las definiciones base que se repiten entre managers, modelos y colisiones;
- integrar una cámara funcional al render del framework;
- encapsular el grid de colisiones de forma que pueda seguir el espacio de trabajo activo del runtime.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Interfaces/`, donde `Definitions.hpp` pasa a reunir tipos base utilizados por entrada, escena, cuerpos, cámara y objetos.
- `src/WandAllegroEngine/Managers/`, donde `CameraManager` entra en operación efectiva, `BodyManager` deja de administrar varias grillas y pasa a encapsular una sola área activa, y `SceneManager` integra el dibujo de cámara dentro del frame.
- `src/WandAllegroEngine/Models/` y `src/WandAllegroEngine/Collisions/`, donde `GameObject`, `GameBodyObject` y `GameGridCollisions` se alinean con las definiciones compartidas del framework.

## Arquitectura o sistemas principales

### Tipos base unificados

En `Definitions.hpp` concentré estructuras y enumeraciones que antes aparecían dispersas o duplicadas en distintas capas del motor. A partir de esta etapa, el framework comparte desde un único punto:

- `Vec2`;
- `Size`;
- `WAND_COLOR`;
- `GameBodyObjectType`;
- `Object`;
- `KeyEventType`.

Con este ajuste dejé una base común para entrada, escena, colisiones, cuerpos, cámara y objetos, reduciendo duplicación y evitando que cada subsistema tenga su propia variante de tipos equivalentes.

### Cámara operativa dentro del runtime

`CameraManager` deja de ser un contenedor vacío y pasa a operar como subsistema real del framework. Su implementación visible incorpora:

- una transformación activa de cámara basada en `ALLEGRO_TRANSFORM`;
- control de posición y tamaño del área visible;
- `FocusObject(GameObject *Obj)` y `FocusPosition(Vec2 NewPosition)` para centrar la vista;
- `Reset()` para reconstruir el estado inicial de cámara a partir del tamaño de ventana;
- `Draw()` para representar referencias visuales de la cámara en pantalla.

Además, `SceneManager::Draw()` ya integra el dibujo de cámara después del render de objetos, con lo que la cámara entra formalmente en el pipeline visual del framework.

### Grid de colisiones encapsulado

En `BodyManager` sustituí el almacenamiento de múltiples grids por una única referencia central `GridArea`. Sobre esa base, el manager ahora expone:

- `GetGridPosition()`;
- `SetGridPosition(Vec2 NewPosition)`.

Con esto el grid de colisiones deja de estar repartido entre varias instancias internas y pasa a poder desplazarse como un área única asociada al estado espacial activo del motor.

### Alineación de modelos y colisiones

`GameBodyObject`, `GameObject` y `GameGridCollisions` se adaptan a la misma base tipada del framework. En especial:

- `GameBodyObject` deja de definir `Vec2` y `GameBodyObjectType` localmente;
- `GameGridCollisions` incorpora acceso explícito a posición mediante `GetPosition()` y `SetPosition()`;
- `GameObject` y `GameBodyObject` quedan encapsulados en `namespace WandEngine`, alineando su interfaz con el resto de managers y modelos.

Con ello dejé mejor acopladas las capas de colisión, cámara y objetos alrededor de una misma representación espacial.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- los managers que trabajan con espacio y render ya comparten las mismas estructuras base para posición, tamaño y color;
- la cámara puede desplazar el área activa de trabajo y reflejar ese cambio en el grid de colisiones y en el dibujo del frame.

Con ello dejé una base más coherente para extender navegación, seguimiento de objetos y manipulación espacial del runtime sin depender de tipos redefinidos en cada subsistema.

## Dependencias externas visibles

No incorporé dependencias externas visibles nuevas en esta etapa.

## Resumen técnico de la versión

La etapa efectiva al **7 de noviembre de 2024** queda delimitada por estos movimientos:

- centralicé en `Definitions.hpp` los tipos base de posición, tamaño, color, entrada y clasificación de objetos y cuerpos;
- activé `CameraManager` como subsistema real del framework con transformación, enfoque y representación visual propia;
- integré el dibujo de cámara al flujo de `SceneManager`;
- encapsulé el grid de colisiones en una única área activa dentro de `BodyManager` y habilité su consulta y desplazamiento;
- adapté `GameBodyObject`, `GameObject` y `GameGridCollisions` al nuevo núcleo compartido de tipos del framework.

Con esta etapa dejé el framework más consistente en sus definiciones básicas y mejor preparado para manejar espacio, cámara y colisiones desde una misma base estructural.