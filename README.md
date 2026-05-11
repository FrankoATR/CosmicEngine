# WandAllegroEngine

## Fecha efectiva

**23 de diciembre de 2024**

## Descripción general

En esta etapa reorganicé la base del proyecto para separar con mayor claridad el framework, las dependencias embebidas y el código específico del juego. El cambio más visible es el traslado del engine hacia un árbol público bajo `include/WandEngine/`, dejando de usar `src/WandAllegroEngine/` como ubicación principal de los componentes del motor.

Sobre esa reestructuración también incorporé una configuración formal con CMake, de modo que la compilación ya no dependa únicamente de tareas manuales o comandos ad hoc y el proyecto pueda describir de forma explícita sus fuentes, bibliotecas enlazadas, salida binaria e icono del ejecutable.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- reubicar el framework en una estructura de carpetas más coherente para consumo e inclusión de cabeceras;
- formalizar el proceso de compilación y enlace mediante CMake;
- alinear el código del juego y las dependencias locales con esa nueva organización del repositorio.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `include/WandEngine/`, donde ahora queda concentrado el código del framework en sus bloques de `Managers`, `Models`, `Collisions` e `Interfaces`, junto con las bibliotecas de Allegro usadas por el motor.
- `src/`, donde pasan a quedar únicamente el punto de entrada y el código específico del juego, distribuido en `Scenes/`, `Entities/`, `Events/` y `Utilities/`.
- `include/sqlite/`, donde se incorpora la dependencia embebida de SQLite como parte del árbol local del proyecto.
- `CMakeLists.txt`, que pasa a describir la construcción completa del ejecutable, incluyendo archivos del engine, archivos del juego, bibliotecas enlazadas, nombre final de salida y recurso de icono.

## Arquitectura o sistemas principales

### Framework reubicado como árbol público

El cambio estructural principal es que el engine deja de vivir bajo `src/WandAllegroEngine/` y pasa a organizarse dentro de `include/WandEngine/`. Desde esta etapa, el framework queda distribuido como un árbol más cercano a una librería reutilizable, donde cabeceras e implementaciones del motor conviven en una estructura pública y homogénea.

La organización visible del engine queda compuesta por:

- `include/WandEngine/Managers/`;
- `include/WandEngine/Models/`;
- `include/WandEngine/Collisions/`;
- `include/WandEngine/Interfaces/`.

Ese ajuste también cambia la forma en que el código del juego consume el engine, pasando a usar includes del tipo `<WandEngine/...>` desde `main.cpp` y las escenas activas.

### Separación entre motor, juego y dependencias

Con la reestructuración anterior, `src/` queda reservado para el código concreto del juego y ya no para mezclar ahí también la implementación del motor. El árbol visible pasa a distinguir:

- framework bajo `include/WandEngine/`;
- dependencias embebidas bajo `include/sqlite/`;
- punto de entrada y contenido del juego bajo `src/`.

Dentro de `src/`, además, el código visible se reorganiza alrededor de entidades y escenas nuevas como `Player`, `Ground`, `SolidBlock`, `Spike` y `Background`, mientras desaparecen varias piezas de ejemplo anteriores ligadas a etapas previas del framework.

### Incorporación formal de CMake

`CMakeLists.txt` entra como descripción explícita del proceso de build del proyecto. La configuración visible define:

- versión mínima de CMake;
- nombre del proyecto;
- nombre final del ejecutable;
- archivo de icono del binario;
- conjunto de fuentes del juego;
- conjunto de fuentes del engine;
- bibliotecas a enlazar;
- directorio de salida del ejecutable.

Con ello la construcción deja de depender solo de tareas externas del editor y pasa a quedar versionada dentro del propio repositorio.

### Ensamblado del engine y del juego

La configuración de CMake no solo crea el target principal, sino que además declara de forma explícita qué archivos del framework forman parte del binario. En esta etapa el engine ya se integra al ejecutable a partir de rutas dentro de `include/WandEngine/`, mientras el contenido específico del juego se agrega desde `src/Scenes/` y `src/Entities/`.

También queda visible el enlace con:

- Allegro monolítico distribuido junto al proyecto;
- `ole32` y `uuid` para el entorno Windows;
- `sqlite3` como dependencia local integrada al build.

### Reorientación del ejemplo principal del proyecto

Aunque el foco de esta documentación sigue siendo el framework, la reestructuración también deja claro que el proyecto base cambia de orientación y reorganiza sus assets, entidades y escena principal alrededor de un prototipo distinto. Eso se refleja en rutas de recursos nuevas, en el `CMakeLists.txt` y en la escena principal actual, que consumen la nueva estructura del engine desde includes públicos.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- el código del juego ya consume el motor mediante rutas públicas bajo `include/WandEngine/` en lugar de depender de una estructura interna incrustada dentro de `src/`;
- la compilación del proyecto queda definida desde CMake con una separación explícita entre fuentes del engine, fuentes del juego, dependencias y salida binaria.

Con ello dejé una base más útil para seguir evolucionando el framework como una unidad separable del juego de ejemplo y para reconstruir el proyecto con un flujo de build más claro y repetible.

## Dependencias externas visibles

En esta etapa queda visible de forma explícita la incorporación de CMake como sistema de build del proyecto y la integración local de `sqlite3` dentro del árbol de dependencias embebidas. También se mantiene el enlace con la biblioteca monolítica de Allegro distribuida junto al repositorio.

## Resumen técnico de la versión

La etapa efectiva al **23 de diciembre de 2024** queda delimitada por estos movimientos:

- reubiqué el framework desde `src/WandAllegroEngine/` hacia un árbol público bajo `include/WandEngine/`;
- separé con mayor claridad engine, código del juego y dependencias embebidas dentro del repositorio;
- incorporé `CMakeLists.txt` como configuración formal de compilación y enlace del proyecto;
- declaré desde CMake las fuentes del engine, las fuentes del juego, el nombre final del ejecutable, el icono y el directorio de salida;
- integré de forma explícita el enlace con Allegro, `sqlite3`, `ole32` y `uuid` dentro del nuevo flujo de build;
- reorganicé el contenido específico del juego para consumir el engine desde includes públicos y una estructura de carpetas más clara.

Con esta etapa dejé el proyecto en una forma más ordenada para distribución, compilación y evolución separada entre el framework y el juego de ejemplo.