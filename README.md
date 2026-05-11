# WandAllegroEngine

## Fecha efectiva

**9 de noviembre de 2024**

## Descripción general

En esta etapa incorporé una capa de interfaz propia al framework y la integré al ciclo normal de actualización y dibujo de las escenas. A partir de este punto el motor ya no depende únicamente de objetos del mundo para renderizar y responder a entrada, sino que también puede administrar elementos de UI con posición, tamaño, jerarquía y comportamiento interactivo.

En paralelo separé el manejo de efectos de sonido respecto a la música persistente y ajusté la coordinación entre escena, cámara e input para que la interacción visual y sonora quede organizada desde subsistemas específicos.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- incorporar una base de interfaz reutilizable dentro del framework;
- separar sonido puntual y música continua en servicios distintos;
- trasladar más control del ciclo de frame y depuración visual hacia la escena activa.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Models/`, donde aparece `UIElement` como base para elementos de interfaz y se agrega `UIElements/UIButton` como primer control interactivo concreto.
- `src/WandAllegroEngine/Managers/`, donde `UIManager` entra en uso efectivo, `SoundManager` se incorpora como servicio nuevo y `SceneManager` redistribuye el control de actualización, dibujo y reemplazo de escenas.
- `src/WandAllegroEngine/Interfaces/` y las capas de input, cámara, objetos, cuerpos y colisiones, donde la nomenclatura compartida de tipos pasa a usar prefijos `WAND_` de forma consistente.

## Arquitectura o sistemas principales

### Capa base de interfaz

Incorporé `UIElement` como clase base para construir interfaz dentro del framework. Su implementación visible resuelve:

- posición y tamaño mediante `WAND_VEC2` y `WAND_SIZE`;
- visibilidad por elemento;
- relación padre e hijos para componer UI jerárquica;
- actualización y dibujo encadenados sobre la jerarquía;
- detección de `MouseHover()` sobre el área ocupada.

Sobre esa base también agregué `UIButton` como primer control concreto, con sprite, texto, fuente, callback de clic y estados básicos de interacción para hover y presión del mouse.

### Manager de UI integrado al frame

`UIManager` entra como singleton efectivo para registrar, actualizar, dibujar y limpiar elementos de interfaz. Además conserva un estado global `MouseHoverAny` para saber si algún elemento está capturando la atención del cursor durante el frame.

Con esto la UI deja de ser una responsabilidad dispersa de escenas o entidades concretas y pasa a contar con un punto central de administración dentro del framework.

### Audio dividido entre música y efectos

Agregué `SoundManager` como servicio específico para efectos de sonido basados en `ALLEGRO_SAMPLE` y `ALLEGRO_SAMPLE_INSTANCE`. Su interfaz visible cubre:

- `Load()`;
- `Play()`;
- `Stop()`;
- `StopAll()`;
- `SetVolume()`;
- `Clear()`.

Al mismo tiempo normalicé `MusicManager` para usar la misma convención de nombres (`Load`, `Play`, `Pause`, `Resume`, `Stop`) y ajusté su inicialización para coexistir mejor con otros servicios de audio cuando Allegro ya se encuentra instalado.

### Escena como orquestador de managers

`GameScene` asume más control directo sobre el frame del runtime mediante dos puntos nuevos:

- `UpdateManagers(double deltaTime)`;
- `Draw()`.

Desde ahí la escena coordina su propia lógica, la actualización de UI, cuerpos y objetos, y también el dibujo opcional de cuerpos y cámara mediante banderas de depuración (`ShowBodys`, `ShowGrid`, `ShowCamera`).

En la misma línea, `SceneManager` deja preparado el reemplazo diferido de escena mediante `NextScene`, delega el frame visible en `GameScene::Draw()` y cierra la ejecución cuando ya no existe una escena activa.

### Input y cámara aplicados a interfaz

La nueva capa UI obligó a ajustar la lectura espacial de entrada. `InputManager` ahora expone `GetMousePosition()` y devuelve la posición del cursor compensada con la posición actual de cámara, mientras `CameraManager` agrega `GetPosition()` para volver accesible esa referencia.

Además, la base tipada compartida se termina de consolidar con el reemplazo visible de `Vec2` y `Size` por `WAND_VEC2` y `WAND_SIZE` en managers, modelos y colisiones.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- la escena activa puede actualizar y dibujar objetos del mundo, depuración visual y elementos de interfaz desde un mismo punto de control;
- el audio queda separado entre música persistente y sonidos de evento, evitando mezclar ambas responsabilidades en un solo manager.

Con ello dejé una base más útil para construir menús, overlays, controles interactivos y retroalimentación sonora sin forzar esa lógica dentro de las entidades del mundo.

## Dependencias externas visibles

No incorporé dependencias externas visibles nuevas en esta etapa.

## Resumen técnico de la versión

La etapa efectiva al **9 de noviembre de 2024** queda delimitada por estos movimientos:

- incorporé `UIElement` y `UIButton` como primera base visible para construir interfaz interactiva dentro del framework;
- integré `UIManager` al ciclo de actualización, dibujo y limpieza de escena;
- añadí `SoundManager` como servicio separado para efectos de sonido y alineé `MusicManager` con una interfaz más uniforme;
- trasladé a `GameScene` el control directo de actualización de managers, dibujo del frame y banderas de depuración visual;
- reorganicé `SceneManager` para reemplazar escenas de forma diferida y cerrar correctamente el runtime cuando no queda ninguna activa;
- consolidé el uso de `WAND_VEC2` y `WAND_SIZE` como convención tipada visible en input, cámara, objetos, cuerpos y colisiones.

Con esta etapa dejé el framework mejor preparado para construir interfaz, respuesta sonora y control de frame desde una arquitectura más clara y separada por responsabilidades.