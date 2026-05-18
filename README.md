# Guia De Configuracion De Proyectos En CosmicEngine

Este documento describe el flujo general necesario para crear, configurar, compilar y ejecutar un proyecto basico dentro del framework CosmicEngine en una maquina limpia. El mismo proceso aplica a cualquier proyecto alojado dentro de `projects/`.

## Estructura Recomendada Del Proyecto

Un proyecto basico debe mantenerse autonomo dentro de `projects/[nombre-del-proyecto]`. Una estructura recomendada es:

```text
projects/
  MyProject/
    project.cmake
    scenes/
      main_scene.hpp
      main_scene.cpp
    entities/
    utilities/
    assets/
      textures/
      audio/
      fonts/
```

Elementos minimos requeridos:

1. Una carpeta del proyecto dentro de `projects/`.
2. Un manifiesto `project.cmake`.
3. Al menos una escena inicial.
4. Un directorio `assets/` cuando el proyecto dependa de recursos externos.

## Requisitos De `project.cmake`

Cada proyecto debe definir su propio `project.cmake`. Como minimo, ese archivo debe declarar:

1. `COSMIC_PROJECT_NAME`
2. `COSMIC_PROJECT_EXECUTABLE_NAME`
3. `COSMIC_PROJECT_MODE`
4. `COSMIC_PROJECT_FIRST_SCENE_HEADER`
5. `COSMIC_PROJECT_FIRST_SCENE_CLASS`
6. `COSMIC_PROJECT_SOURCE_FILES`

Ejemplo:

```cmake
set(COSMIC_PROJECT_NAME "MyProject")
set(COSMIC_PROJECT_EXECUTABLE_NAME "MyProject")
set(COSMIC_PROJECT_MODE "GAME_2D_CONFIGURATION")
set(COSMIC_PROJECT_FIRST_SCENE_HEADER "scenes/main_scene.hpp")
set(COSMIC_PROJECT_FIRST_SCENE_CLASS "MainScene")

set(COSMIC_PROJECT_SOURCE_FILES
    scenes/main_scene.cpp
)
```

Los proyectos tambien pueden definir valores opcionales como resolucion inicial, modo fullscreen, VSync, titulo de ventana, directorio de assets y ruta de icono.

## Seleccion Del Proyecto

La compilacion raiz compila un proyecto a la vez mediante la variable de cache `COSMIC_PROJECT`. Si la carpeta del proyecto es `projects/MyProject`, el valor seleccionado debe ser `MyProject`.

## Requisitos En Windows

Entorno recomendado: `MSYS2 UCRT64`.

Paquetes minimos a instalar:

```bash
pacman -Syu
pacman -S --needed \
  mingw-w64-ucrt-x86_64-gcc \
  mingw-w64-ucrt-x86_64-cmake \
  mingw-w64-ucrt-x86_64-make
```

Herramientas que deben estar disponibles en `PATH`:

1. `gcc`
2. `g++`
3. `cmake`
4. `mingw32-make`

Comprobacion rapida:

```bash
gcc --version
g++ --version
cmake --version
mingw32-make --version
```

Notas importantes sobre MSYS2:

1. Usa una terminal `MSYS2 UCRT64`, o una terminal de VS Code que cargue ese mismo entorno.
2. En este entorno el comando normal de make es `mingw32-make`, no `make`.
3. Si `cmake --version` falla o no imprime nada, la causa habitual es que falte `mingw-w64-ucrt-x86_64-cmake` o que la terminal no este ejecutandose bajo `UCRT64`.

Los presets de Windows en este repositorio usan el generador `MinGW Makefiles`. Las librerias estaticas vendorizadas ya estan almacenadas en `lib/`, por lo que no necesitan instalarse por separado en la maquina.

Durante la compilacion en Windows, el framework tambien copia las DLL del runtime de MinGW y cualquier DLL encontrada en `dll/` hacia el directorio final del ejecutable.

## Requisitos En Linux

Para configurar y compilar en Linux, se requieren las siguientes herramientas:

1. Git.
2. CMake.
3. Un toolchain C/C++ como `gcc` y `g++`.
4. `make`.
5. Paquetes del sistema para OpenGL, Freetype, Zlib, Pkg-config, GLFW3 y Assimp.

Configuracion tipica en Debian o Ubuntu:

```sh
sudo apt update
sudo apt install build-essential cmake pkg-config libglfw3-dev libfreetype6-dev zlib1g-dev libassimp-dev libgl1-mesa-dev
```

Los nombres de paquetes pueden variar segun la distribucion Linux.

## Procedimiento De Compilacion En Windows

La compilacion en Windows se realiza desde la raiz del repositorio mediante presets de CMake orientados por proyecto. No hace falta entrar manualmente a `projects/<demo>/build`.

1. Abre una terminal `MSYS2 UCRT64`.
2. Ve a la raiz del repositorio.
3. Configura usando el preset del proyecto y configuracion deseados.
4. Compila con el build preset correspondiente.

Ejemplo para `sandbox_3D` en release:

```bash
cd CosmicEngine
cmake --preset windows-release-sandbox-3d
cmake --build --preset build-windows-release-sandbox-3d
```

Ejemplo para `NetworkDemo` en debug:

```bash
cd CosmicEngine
cmake --preset windows-debug-networkdemo
cmake --build --preset build-windows-debug-networkdemo
```

Ejemplo equivalente en PowerShell:

```powershell
cd CosmicEngine
cmake --preset windows-release-networkdemo
cmake --build --preset build-windows-release-networkdemo
```

Los presets publicos disponibles en Windows son:

1. `windows-release-networkdemo`
2. `windows-debug-networkdemo`
3. `windows-release-basicdemo-2d`
4. `windows-debug-basicdemo-2d`
5. `windows-release-basicdemo-3d`
6. `windows-debug-basicdemo-3d`
7. `windows-release-sandbox-3d`
8. `windows-debug-sandbox-3d`
9. `build-windows-release-networkdemo`
10. `build-windows-debug-networkdemo`
11. `build-windows-release-basicdemo-2d`
12. `build-windows-debug-basicdemo-2d`
13. `build-windows-release-basicdemo-3d`
14. `build-windows-debug-basicdemo-3d`
15. `build-windows-release-sandbox-3d`
16. `build-windows-debug-sandbox-3d`

Estos nombres corresponden a los proyectos de ejemplo incluidos actualmente en el repositorio. Si se crea un proyecto nuevo dentro de `projects/`, puede seguirse la misma estructura de presets cuando se necesite un arbol de build dedicado por proyecto, por ejemplo:

1. `windows-release-myproject`
2. `windows-debug-myproject`
3. `build-windows-release-myproject`
4. `build-windows-debug-myproject`

La recomendacion es conservar el mismo patron: un preset de configure por proyecto y configuracion, y un build preset asociado que reutilice ese configure preset.

Cada preset usa su propio directorio de build, por lo que cambiar entre proyectos no requiere reutilizar el mismo cache ni ejecutar `--fresh` como paso normal.

## Procedimiento De Compilacion En Linux

La compilacion en Linux sigue el mismo modelo por proyecto y usa `Unix Makefiles`. Si mas adelante se agregan presets publicos de Linux, el flujo recomendado es el mismo: un preset de configure por proyecto y otro de build asociado.

Mientras no existan presets publicos de Linux para todos los proyectos, puedes seguir usando los presets base ocultos como referencia para extender `CMakePresets.json` con el mismo patron utilizado en Windows.

## Estructura De Salida En Ejecucion

Las salidas ejecutables finales se escriben en `bin/` usando como componentes de ruta el proyecto, la plataforma y la configuracion de compilacion:

```text
bin/[project-name]/[platform]/[configuration]/
```

Ejemplos:

1. `bin/MyProject/windows/Debug/`
2. `bin/MyProject/windows/Release/`
3. `bin/MyProject/linux/Debug/`
4. `bin/MyProject/linux/Release/`

Los assets del proyecto se copian automaticamente al directorio de salida en tiempo de compilacion.

## Problemas Comunes En Windows

### `make` no existe

Usa `mingw32-make` o, preferiblemente, compila mediante:

```bash
cmake --build --preset build-windows-release
```

### `cmake` no responde o falla al iniciar

Reinstala CMake desde el entorno `UCRT64`:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-cmake
```

### Se compila otra demo distinta

Verifica que el valor enviado en `COSMIC_PROJECT` corresponda al proyecto esperado durante la configuracion.

## Ejecucion Del Proyecto

Las aplicaciones deben ejecutarse desde su directorio de runtime generado para que las rutas relativas a los assets copiados y a las DLL del runtime sigan siendo validas.

Ejemplo en Windows:

```powershell
.\bin\MyProject\windows\Debug\MyProject.exe
```

Ejemplo en Linux:

```sh
./bin/MyProject/linux/Debug/MyProject
```

## Flujo Resumido

El flujo general para crear y levantar un proyecto basico es:

1. Abre una terminal `MSYS2 UCRT64` en Windows, o la shell correspondiente en Linux.
2. Ve a la raiz del repositorio.
3. Crea una nueva carpeta dentro de `projects/`.
4. Agrega `project.cmake`.
5. Implementa la escena inicial y declara sus archivos fuente.
6. Agrega un directorio `assets/` si el proyecto depende de recursos en runtime.
7. Registra o reutiliza presets de CMake por proyecto cuando sea necesario.
8. Configura con el preset del proyecto deseado.
9. Compila con el build preset correspondiente.
10. Ejecuta el binario desde el directorio generado dentro de `bin/`.
